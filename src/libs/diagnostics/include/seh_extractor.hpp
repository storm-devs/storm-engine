// ReSharper disable CppClangTidyClangDiagnosticLanguageExtensionToken

#pragma once

#include <ehdata.h>

class [[nodiscard]] seh_extractor {
public:
    typedef void (*sink_func)(const char *);

    explicit seh_extractor(const EXCEPTION_POINTERS * ep);

    // test if exception is abnormal
    [[nodiscard]] bool is_abnormal() const;

    // printout exception data to the provided sink
    void sink(sink_func f) const;

  private:
    void sink(sink_func f, EXCEPTION_RECORD *next) const;

    const EXCEPTION_POINTERS* ep_{};
    DWORD code_{};
};