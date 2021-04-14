#pragma once

#include <exception>
#include <stdexcept>
#ifdef _WIN32
#include <Windows.h>
#include <array>
#include <fmt/format.h>
#include <string_view>
#include <tuple>
#ifndef _WIN64
#include <errhandlingapi.h>
#endif
#include <eh.h>
#else
#include <csignal>
#endif

namespace storm::except
{
inline void debug_break()
{
#if defined _WIN32 && not defined _WIN64
    SetErrorMode(SEM_FAILCRITICALERRORS);
#endif
#ifdef _WIN32
    __debugbreak();
    /* to get __debugbreak prompt since windows 10 make sure that
     * REG:\HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AeDebug\Auto:REG_SZ == "1"
     */
#else
    raise(SIGABRT);
#endif
}

#ifdef _WIN32
namespace detail
{
// exception defines parsing
template <std::size_t N> constexpr auto split_args(std::string_view s)
{
    std::array<std::string_view, N> arr{};

    std::size_t begin{0}, end{0};
    for (std::size_t i = 0; i < N && end != std::string_view::npos; ++i)
    {
        end = s.find_first_of(',', begin);
        arr[i] = s.substr(begin, end - begin);
        // on windows 'min' is a macro
        arr[i].remove_prefix((std::min)(arr[i].find_first_not_of(' '), arr[i].size()));
        begin = end + 1;
    }

    return arr;
}

template <std::size_t N, int... Values, std::size_t... I>
constexpr auto get_array(std::array<std::string_view, N> strings, std::index_sequence<I...>)
{
    return std::array<std::pair<std::string_view, int>, N>{std::make_pair(strings[I], Values)...};
}

} // namespace detail

class system_exception final : public std::exception
{
  public:
    system_exception() noexcept : system_exception{0, nullptr}
    {
    }

    system_exception(unsigned int err_code, PEXCEPTION_POINTERS ep) noexcept : err_code_{err_code}, ep_(ep)
    {
    }

    [[nodiscard]] const char *what() const noexcept override
    {

#define STORM_EXCEPTIONS_EXPAND(x) x
#define STORM_EXCEPTIONS_VA_ARGS_SIZE(...) std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value
#define STORM_EXCEPTIONS_GET_EXCEPT_ARR(...)                                                                           \
    detail::get_array<STORM_EXCEPTIONS_VA_ARGS_SIZE(__VA_ARGS__), __VA_ARGS__>(                                        \
        detail::split_args<STORM_EXCEPTIONS_VA_ARGS_SIZE(__VA_ARGS__)>(STORM_EXCEPTIONS_EXPAND(#__VA_ARGS__)),         \
        std::make_index_sequence<STORM_EXCEPTIONS_VA_ARGS_SIZE(__VA_ARGS__)>{})

        const auto exceptions{STORM_EXCEPTIONS_GET_EXCEPT_ARR(
            EXCEPTION_DATATYPE_MISALIGNMENT, EXCEPTION_BREAKPOINT, EXCEPTION_SINGLE_STEP,
            EXCEPTION_ARRAY_BOUNDS_EXCEEDED, EXCEPTION_FLT_DENORMAL_OPERAND, EXCEPTION_FLT_DIVIDE_BY_ZERO,
            EXCEPTION_FLT_INEXACT_RESULT, EXCEPTION_FLT_INVALID_OPERATION, EXCEPTION_FLT_OVERFLOW,
            EXCEPTION_FLT_STACK_CHECK, EXCEPTION_FLT_UNDERFLOW, EXCEPTION_INT_DIVIDE_BY_ZERO, EXCEPTION_INT_OVERFLOW,
            EXCEPTION_PRIV_INSTRUCTION, EXCEPTION_IN_PAGE_ERROR, EXCEPTION_ILLEGAL_INSTRUCTION,
            EXCEPTION_NONCONTINUABLE_EXCEPTION, EXCEPTION_STACK_OVERFLOW, EXCEPTION_INVALID_DISPOSITION,
            EXCEPTION_GUARD_PAGE, EXCEPTION_INVALID_HANDLE,
            /*EXCEPTION_POSSIBLE_DEADLOCK,*/ CONTROL_C_EXIT)};

#undef STORM_EXCEPTIONS_GET_EXCEPT_ARR
#undef STORM_EXCEPTIONS_VA_ARGS_SIZE
#undef STORM_EXCEPTIONS_EXPAND

        // get exception message
        std::string_view msg = "Unknown SEH exception";
        for (auto &[name, code] : exceptions)
        {
            if (code == err_code_)
            {
                msg = name;
                break;
            }
        }

        // format into char array without allocations
        constexpr size_t msg_max_size = 128;
        constexpr size_t code_max_size = 1 + 2 + 8 + 1; // ( + 0x + 00000000 + )
        static char ret[msg_max_size + code_max_size + 1];
        size_t i{0};
        for (; i != std::size(msg) && i != msg_max_size; ++i)
        {
            ret[i] = msg[i];
        }
        fmt::format_to(&ret[i], "({:#x})", err_code_);

        return ret;
    }

    auto get_exception_pointers() const
    {
        return ep_;
    }

  private:
    const unsigned int err_code_;
    PEXCEPTION_POINTERS ep_;
};

class scoped_exception_guard
{
  public:
    scoped_exception_guard() noexcept
        : original_translator_{
              _set_se_translator([](const unsigned int u, PEXCEPTION_POINTERS ep) { throw system_exception(u, ep); })}
    {
    }

    ~scoped_exception_guard() noexcept
    {
        _set_se_translator(original_translator_);
    }

  private:
    const _se_translator_function original_translator_;
};

inline void get_exception_pointers(EXCEPTION_POINTERS &ep)
{
    // The following code was taken from VC++ 8.0 CRT (invarg.c: line 104)
    CONTEXT ContextRecord;
    ZeroMemory(&ContextRecord, sizeof(CONTEXT));

#ifdef _X86_

    __asm {
	mov dword ptr [ContextRecord.Eax], eax
		mov dword ptr [ContextRecord.Ecx], ecx
		mov dword ptr [ContextRecord.Edx], edx
		mov dword ptr [ContextRecord.Ebx], ebx
		mov dword ptr [ContextRecord.Esi], esi
		mov dword ptr [ContextRecord.Edi], edi
		mov word ptr [ContextRecord.SegSs], ss
		mov word ptr [ContextRecord.SegCs], cs
		mov word ptr [ContextRecord.SegDs], ds
		mov word ptr [ContextRecord.SegEs], es
		mov word ptr [ContextRecord.SegFs], fs
		mov word ptr [ContextRecord.SegGs], gs
		pushfd
		pop [ContextRecord.EFlags]
    }

    ContextRecord.ContextFlags = CONTEXT_CONTROL;
#pragma warning(push)
#pragma warning(disable : 4311)
    ContextRecord.Eip = (ULONG)_ReturnAddress();
    ContextRecord.Esp = (ULONG)_AddressOfReturnAddress();
#pragma warning(pop)
    ContextRecord.Ebp = *((ULONG *)_AddressOfReturnAddress() - 1);

#elif defined(_IA64_) || defined(_AMD64_)

    /* Need to fill up the Context in IA64 and AMD64. */
    RtlCaptureContext(&ContextRecord);

#else /* defined (_IA64_) || defined (_AMD64_) */

    ZeroMemory(&ContextRecord, sizeof(ContextRecord));

#endif /* defined (_IA64_) || defined (_AMD64_) */

    EXCEPTION_RECORD ExceptionRecord;
    ZeroMemory(&ExceptionRecord, sizeof(EXCEPTION_RECORD));

    ExceptionRecord.ExceptionCode = 0;
    ExceptionRecord.ExceptionAddress = _ReturnAddress();

    EXCEPTION_RECORD *pExceptionRecord = new EXCEPTION_RECORD;
    memcpy(pExceptionRecord, &ExceptionRecord, sizeof(EXCEPTION_RECORD));
    CONTEXT *pContextRecord = new CONTEXT;
    memcpy(pContextRecord, &ContextRecord, sizeof(CONTEXT));

    ep.ExceptionRecord = pExceptionRecord;
    ep.ContextRecord = pContextRecord;
}

#else
struct system_exception : public std::exception
{
    const char *what() const noexcept override
    {
        return "system_exception not implemented";
    }
    int get_code()
    {
        return -1;
    }
};
struct scoped_exception_guard
{
};
#endif
} // namespace storm::except
