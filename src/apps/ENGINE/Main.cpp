#include "externs.h"
#include "fs.h"
#include "s_debug.h"
#include "SteamApi.hpp"
#include "compiler.h"

#include <crtdbg.h>
#include <dbghelp.h>
#include <tchar.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>


#include "bx/bx.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"

#include "sprite_renderer.h"

#include "common.h"

#include "dx9render.h"

constexpr auto DUMP_FILENAME = "engine_dump.dmp";

S_DEBUG CDebug;
bool isHold = false;
// LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int Alert(const char *lpCaption, const char *lpText);
void CreateMiniDump(EXCEPTION_POINTERS *pep);
bool _loopMain();

bool _loopMain()
{
    bool runResult = false;

    __try
    {
        runResult = core.Run();
    }
    __except (CreateMiniDump(GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER)
    {
    }

    return runResult;
}

bgfx::PlatformData *platformData;

namespace bgfx
    {
    extern bgfx::PlatformData g_platformData;

    void LoadPlatformData() // because outside the namespace will generate link errors;
    {
        platformData = &g_platformData;
    }
}



namespace storm
{

#define MAX_WINDOWS 1


class Application : public entry::AppI
{
  public:
    Application(const char *_name, const char *_description, const char *_url, long screen_x, long screen_y)
        : entry::AppI(_name, _description, _url)
    {
        m_width = screen_x;
        m_height = screen_y;

    }

    void init(int32_t _argc, const char *const *_argv, uint32_t _width, uint32_t _height) override
    {
        bgfx::LoadPlatformData();

        core.Set_Hwnd((HWND)platformData->nwh);

        Args args(_argc, _argv);

        m_debug = BGFX_DEBUG_TEXT;
        m_reset = BGFX_RESET_VSYNC;

        bgfx::Init init;
        init.type = bgfx::RendererType::Direct3D9;
        init.vendorId = args.m_pciId;
        init.resolution.width = m_width;
        init.resolution.height = m_height;
        init.resolution.reset = m_reset;

        bgfx::init(init);

        const bgfx::Caps *caps = bgfx::getCaps();
        //bool swapChainSupported = 0 != (caps->supported & BGFX_CAPS_SWAP_CHAIN);


        // Enable m_debug text.
        bgfx::setDebug(m_debug);

        // Set view 0 clear state.
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
        //bgfx::setViewClear(0, BGFX_CLEAR_NONE, 0x303030ff, 1.0f, 0);

        // Init stuff
        core.InitBase();

        core.AppState(true);

        _loopMain(); // to allow service initialization, remove/comment after tests

        m_renderService = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
        if (m_renderService == nullptr)
            throw std::exception("!Butterflies: No service 'dx9render'");


    }


    bool update() override
    {
        if (!entry::processWindowEvents(m_state, m_debug, m_reset))
        {
            entry::MouseState mouseState = m_state.m_mouse;          

            bgfx::touch(0);

            _loopMain();

            bgfx::frame();
            //bgfx::renderFrame();
            return true;
        }

        return false;
    }

    virtual int shutdown() override
    {

        // Release
        core.ReleaseBase();
        ClipCursor(nullptr);


        // Shutdown bgfx.
        bgfx::shutdown();

        return 0;
    }

    entry::WindowState m_state;

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_debug;
    uint32_t m_reset;

    //SpriteRenderer m_spriteRenderer;

    std::shared_ptr<TextureResource> m_texture;
    std::shared_ptr<TextureResource> m_texture2;

    long PortraitID;

    long PortraitID;

    VDX9RENDER *m_renderService;
};

} // namespace

/*ENTRY_IMPLEMENT_MAIN(bgfx::ExampleWindows, "Sea Dogs", "Rendering test",
                     "https://github.com/storm-devs/storm-engine");*/

int _main_(int _argc, char **_argv)
{
    
    fio = &File_Service;
    //_VSYSTEM_API = &System_Api;

    // Init stash
    create_directories(fs::GetLogsPath());
    create_directories(fs::GetSaveDataPath());

    // Delete old dump file
    std::filesystem::path log_path = fs::GetStashPath() / std::filesystem::u8path(DUMP_FILENAME);
    fio->_DeleteFile(log_path.string().c_str());

    // Init system log
    log_path = fs::GetLogsPath() / std::filesystem::u8path("system.log");
    fio->_DeleteFile(log_path.string().c_str());
    core.tracelog = spdlog::basic_logger_mt("system", log_path.string(), true);
    spdlog::set_default_logger(core.tracelog);
    core.tracelog->set_level(spdlog::level::trace);

    // Init compile and error/warning logs
    log_path = fs::GetLogsPath() / std::filesystem::u8path(COMPILER_LOG_FILENAME);
    fio->_DeleteFile(log_path.string().c_str());
    core.Compiler->tracelog = spdlog::basic_logger_mt("compile", log_path.string(), true);
    core.Compiler->tracelog->set_level(spdlog::level::trace);
    log_path = fs::GetLogsPath() / std::filesystem::u8path(COMPILER_ERRORLOG_FILENAME);
    fio->_DeleteFile(log_path.string().c_str());
    core.Compiler->error_warning_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_path.string());
    core.Compiler->errorlog = std::make_shared<spdlog::logger>("error", core.Compiler->error_warning_sink);
    core.Compiler->errorlog->set_level(spdlog::level::trace);
    core.Compiler->warninglog = std::make_shared<spdlog::logger>("warning", core.Compiler->error_warning_sink);
    core.Compiler->warninglog->set_level(spdlog::level::trace);

    // Read config
    uint32_t dwMaxFPS = 0;
    auto *ini = File_Service.OpenIniFile(ENGINE_INI_FILE_NAME);
    bool bSteam = false;

    long screen_x = 1920;
    long screen_y = 1080;

    if (ini)
    {
        dwMaxFPS = static_cast<uint32_t>(ini->GetLong(nullptr, "max_fps", 0));
        auto bDebugWindow = ini->GetLong(nullptr, "DebugWindow", 0) == 1;
        auto bAcceleration = ini->GetLong(nullptr, "Acceleration", 0) == 1;
        if (!ini->GetLong(nullptr, "logs", 0))
        {
            core.tracelog->set_level(spdlog::level::off);
        }
        if (ini->GetLong(nullptr, "Steam", 1) != 0)
        {
            bSteam = true;
        }
        else
        {
            bSteam = false;
        }

        screen_x = ini->GetLong(nullptr, "screen_x", 1);
        screen_y = ini->GetLong(nullptr, "screen_y", 1);
        delete ini;
    }

    // evaluate SteamApi singleton
    steamapi::SteamApi::getInstance(!bSteam);

    
    entry::s_width = screen_x;
    entry::s_height = screen_y;



    storm::Application app("Sea Dogs", "Rendering test", "https://github.com/storm-devs/storm-engine", screen_x, screen_y);

    return entry::runApp(&app, _argc, _argv);
}


void CreateMiniDump(EXCEPTION_POINTERS *pep)
{
    std::filesystem::path dmpfile = fs::GetStashPath() / std::filesystem::u8path(DUMP_FILENAME);
    // Open the file
    HANDLE hFile =
        CreateFile(dmpfile.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
    {
        // Create the minidump
        MINIDUMP_EXCEPTION_INFORMATION mdei;

        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = pep;
        mdei.ClientPointers = FALSE;

        MINIDUMP_TYPE mdt =
            (MINIDUMP_TYPE)(MiniDumpWithFullMemory | MiniDumpWithFullMemoryInfo | MiniDumpWithHandleData |
                            MiniDumpWithThreadInfo | MiniDumpWithUnloadedModules);

        BOOL rv =
            MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, mdt, (pep != 0) ? &mdei : 0, 0, 0);

        if (!rv)
            _tprintf(_T("MiniDumpWriteDump failed. Error: %u \n"), GetLastError());
        else
            _tprintf(_T("Minidump created.\n"));

        // Close the file
        CloseHandle(hFile);
    }
    else
    {
        _tprintf(_T("CreateFile failed. Error: %u \n"), GetLastError());
    }
}

int Alert(const char *lpCaption, const char *lpText)
{
    std::wstring CaptionW = utf8::ConvertUtf8ToWide(lpCaption);
    std::wstring TextW = utf8::ConvertUtf8ToWide(lpText);
    return ::MessageBox(NULL, TextW.c_str(), CaptionW.c_str(), MB_OK);
}
