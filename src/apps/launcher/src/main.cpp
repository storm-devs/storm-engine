#include <array>
#include <functional>
#include <string>
#include <filesystem>
#include <fstream>
#include <format>

#include <SDL2/SDL.h>
#include <boost/asio.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/process/v2/start_dir.hpp>
#include <cpuinfo.h>

namespace
{

std::filesystem::path g_log;

void log(std::string_view msg)
{
    if (g_log.empty())
    {
        g_log = std::tmpnam(nullptr);
    }

    static std::ofstream out(g_log);
    if (out)
    {
        out << msg << std::endl;
    }
}

struct SortedCaps
{
    std::move_only_function<bool()> predicate{[] { return true; }};
    std::string value;
};

std::array g_caps{SortedCaps{[] {
                                 return cpuinfo_has_x86_avx512f() && cpuinfo_has_x86_avx512cd() &&
                                        cpuinfo_has_x86_avx512bw() && cpuinfo_has_x86_avx512dq() &&
                                        cpuinfo_has_x86_avx512vl();
                             },
                             "AVX512"},
                  SortedCaps{cpuinfo_has_x86_avx2, "AVX2"}, SortedCaps{cpuinfo_has_x86_avx2, "AVX"},
                  SortedCaps{.value = "SSE2"}};

std::string getArchExt(size_t fallback_level)
{
    for (size_t i = 0; i < std::size(g_caps); ++i)
    {
        if (g_caps[i].predicate())
        {
            if (fallback_level == 0)
            {
                return g_caps[i].value;
            }
            --fallback_level;
        }
    }

    throw std::runtime_error("Error: no suitable binary can be launched");
}

std::string getArch(size_t fallback_level)
{
#if CPUINFO_ARCH_X86_64
    return "x86-64-" + getArchExt(fallback_level);
#else
#error Current host arch not implemented yet!
#endif
}

} // namespace

int main(int argc, char *argv[]) try
{
    cpuinfo_initialize();

    namespace bp = boost::process::v2;
    bp::default_process_launcher launcher;
    boost::asio::io_context ctx;
    boost::system::error_code ec;

    int fallback_level{0};
    do
    {
        std::string binary_name = std::string(SDL_GetBasePath()) + "bin\\" + getArch(fallback_level) + ".exe";
        log(std::format("Trying to run {} with fallback_level={}", binary_name, fallback_level));
        auto proc = launcher(ctx, ec, binary_name.c_str(), std::vector<std::string>{}).detach();
        ++fallback_level;

        log(ec.message());
    } while (ec.failed());

    return EXIT_SUCCESS;
}
catch (std::exception & e)
{
    log(e.what());
    #ifdef _WINDOWS
        std::system(("notepad.exe \"" + g_log.string() + "\"").c_str());
#endif

    return EXIT_FAILURE;
}