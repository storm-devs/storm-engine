#include "ExecutableLifecycleService.hpp"
#include "SteamApi.hpp"
#include "compiler.h"
#include "file_service.h"
#include "fs.h"
#include "s_debug.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <OSWindow.hpp>
#include <SDL.h>
#include "bgfx_utils.h"
#include "common.h"
#include "imgui/imgui.h"
#include <entry/input.h>


constexpr auto DUMP_FILENAME = "engine_dump.dmp";

VFILE_SERVICE *fio = nullptr;
CORE core;
S_DEBUG CDebug;

using namespace storm;
bool isHold = false;
// LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int Alert(const char *lpCaption, const char *lpText);
void CreateMiniDump(EXCEPTION_POINTERS *pep);
bool _loopMain();

namespace
{

bool isHold = false;
bool isRunning = false;
bool bActive = true;

} // namespace

void HandleWindowEvent(const OSWindow::Event &event)
{
    if (event == OSWindow::Closed)
    {
        isRunning = false;
        core.Event("DestroyWindow", nullptr);
    }
    else if (event == OSWindow::FocusGained)
    {
        bActive = true;
        core.AppState(bActive);
    }
    else if (event == OSWindow::FocusLost)
    {
        bActive = false;
        core.AppState(bActive);
    }
}

void cmdCreateWindow(const void *_userData);
void cmdDestroyWindow(const void *_userData);

namespace bgfx
{
    // Prevent multiple instances
    if (!CreateEventA(nullptr, false, false, "Global\\FBBD2286-A9F1-4303-B60C-743C3D7AA7BE") ||
        GetLastError() == ERROR_ALREADY_EXISTS)

#define MAX_WINDOWS 1

struct PosColorVertex
{
    float m_x;
    float m_y;
    float m_z;
    uint32_t m_abgr;

    static void init()
    {
        ms_layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
    };

    static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout PosColorVertex::ms_layout;

static PosColorVertex s_cubeVertices[8] = {
    {-1.0f, 1.0f, 1.0f, 0xff000000},   {1.0f, 1.0f, 1.0f, 0xff0000ff},   {-1.0f, -1.0f, 1.0f, 0xff00ff00},
    {1.0f, -1.0f, 1.0f, 0xff00ffff},   {-1.0f, 1.0f, -1.0f, 0xffff0000}, {1.0f, 1.0f, -1.0f, 0xffff00ff},
    {-1.0f, -1.0f, -1.0f, 0xffffff00}, {1.0f, -1.0f, -1.0f, 0xffffffff},
};

static const uint16_t s_cubeIndices[36] = {
    0, 1, 2,          // 0
    1, 3, 2, 4, 6, 5, // 2
    5, 6, 7, 0, 2, 4, // 4
    4, 2, 6, 1, 5, 3, // 6
    5, 7, 3, 0, 4, 1, // 8
    4, 5, 1, 2, 3, 6, // 10
    6, 3, 7,
};

extern bgfx::PlatformData g_platformData;

class ExampleWindows : public entry::AppI
{
  private:
    bool initiated;

  public:
    ExampleWindows(const char *_name, const char *_description, const char *_url)
        : entry::AppI(_name, _description, _url)
    {
        initiated = false;
    }

    SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

    // Init FS
    FILE_SERVICE File_Service;
    fio = &File_Service;

    // Init stash
    create_directories(fs::GetLogsPath());
    create_directories(fs::GetSaveDataPath());

    // Init logging
    auto systemLogPath = fs::GetLogsPath() / u8"system.log";
    auto systemLog = spdlog::basic_logger_st("system", systemLogPath.string(), true);
    systemLog->flush_on(spdlog::level::critical);
    set_default_logger(systemLog);

    auto compileLogPath = fs::GetLogsPath() / COMPILER_LOG_FILENAME;
    auto compileLog = spdlog::basic_logger_st("compile", compileLogPath.string(), true);

    auto errorLogPath = fs::GetLogsPath() / COMPILER_ERRORLOG_FILENAME;
    auto errorLog = spdlog::basic_logger_st("error", errorLogPath.string(), true);

    std::set_terminate(spdlog::shutdown);
    std::atexit(spdlog::shutdown);

    // Init executable lifecycle manager
    storm::except::ExecutableLifecycleService lifecycleService;
    lifecycleService.addAttachment(systemLogPath);
    lifecycleService.addAttachment(compileLogPath);
    lifecycleService.addAttachment(errorLogPath);
    if (!lifecycleService.initialize(true))
    {
        spdlog::error("Unable to initialize crash handler");
    }

    // Init core
    core.Init();
    core.tracelog = systemLog;
    core.Compiler->tracelog = compileLog;
    core.Compiler->errorlog = errorLog;
    core.Compiler->warninglog = errorLog;

    // Init script debugger
    CDebug.Init();

    /* Read config */
    uint32_t dwMaxFPS = 0;
    auto ini = fio->OpenIniFile(fs::ENGINE_INI_FILE_NAME);
    bool bSteam = false;
    int width = 1024, height = 768;
    bool fullscreen = false;

    if (ini)
    
    void createWindow()
    {
        entry::WindowHandle handle = entry::createWindow(rand() % 1280, rand() % 720, 640, 480);
        if (entry::isValid(handle))
        {
            char str[256];
            bx::snprintf(str, BX_COUNTOF(str), "Window - handle %d", handle.idx);
            entry::setWindowTitle(handle, str);
            m_windows[handle.idx].m_handle = handle;
        }
    }

    void destroyWindow()
    {
        for (uint32_t ii = 0; ii < MAX_WINDOWS; ++ii)
        {
            if (bgfx::isValid(m_fbh[ii]))
            {
                bgfx::destroy(m_fbh[ii]);
                m_fbh[ii].idx = bgfx::kInvalidHandle;

                // Flush destruction of swap chain before destroying window!
                bgfx::frame();
                bgfx::frame();
            }

            if (entry::isValid(m_windows[ii].m_handle))
            {
                entry::destroyWindow(m_windows[ii].m_handle);
                m_windows[ii].m_handle.idx = UINT16_MAX;
                return;
            }
        }
    }


    void init(int32_t _argc, const char *const *_argv, uint32_t _width, uint32_t _height) override
    {
        core.Set_Hwnd((HWND)g_platformData.nwh);

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

            delete ini;
        }

        // evaluate SteamApi singleton
        steamapi::SteamApi::getInstance(!bSteam);


        Args args(_argc, _argv);

        m_width = _width;
        m_height = _height;
        m_debug = BGFX_DEBUG_TEXT;
        m_reset = BGFX_RESET_VSYNC;

        bgfx::Init init;
        init.type = args.m_type;
        init.vendorId = args.m_pciId;
        init.resolution.width = m_width;
        init.resolution.height = m_height;
        init.resolution.reset = m_reset;
        bgfx::init(init);

        const bgfx::Caps *caps = bgfx::getCaps();
        bool swapChainSupported = 0 != (caps->supported & BGFX_CAPS_SWAP_CHAIN);

        if (swapChainSupported)
        {
            m_bindings = (InputBinding *)BX_ALLOC(entry::getAllocator(), sizeof(InputBinding) * 3);
            m_bindings[0].set(entry::Key::KeyC, entry::Modifier::None, 1, cmdCreateWindow, this);
            m_bindings[1].set(entry::Key::KeyD, entry::Modifier::None, 1, cmdDestroyWindow, this);
            m_bindings[2].end();

            inputAddBindings("Sea Dogs", m_bindings);
        }
        else
        {
            m_bindings = NULL;
        }

        // Enable m_debug text.
        bgfx::setDebug(m_debug);

        // Set view 0 clear state.
        //bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);

        // Create vertex stream declaration.
        PosColorVertex::init();

        // Create static vertex buffer.
        m_vbh = bgfx::createVertexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)), PosColorVertex::ms_layout);

        // Create static index buffer.
        m_ibh = bgfx::createIndexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(s_cubeIndices, sizeof(s_cubeIndices)));

        // Create program from shaders.
        /*m_program = loadProgram("vs_cubes", "fs_cubes");*/

        m_timeOffset = bx::getHPCounter();

        bx::memSet(m_fbh, 0xff, sizeof(m_fbh));

        imguiCreate();

                // Init stuff
        core.InitBase();

        initiated = true;

        core.AppState(true);

    }


    bool update() override
    {
        if (!entry::processWindowEvents(m_state, m_debug, m_reset))
        {
            entry::MouseState mouseState = m_state.m_mouse;          

            /*imguiBeginFrame(mouseState.m_mx, mouseState.m_my,
                            (mouseState.m_buttons[entry::MouseButton::Left] ? IMGUI_MBUT_LEFT : 0) |
                                (mouseState.m_buttons[entry::MouseButton::Right] ? IMGUI_MBUT_RIGHT : 0) |
                                (mouseState.m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0),
                            mouseState.m_mz, uint16_t(m_width), uint16_t(m_height));

            showExampleDialog(this);

            imguiEndFrame();*/

            if (isValid(m_state.m_handle))
            {
                if (0 == m_state.m_handle.idx)
                {
                    m_width = m_state.m_width;
                    m_height = m_state.m_height;
                }
                else
                {
                    uint8_t viewId = (uint8_t)m_state.m_handle.idx;
                    entry::WindowState &win = m_windows[viewId];

                    if (win.m_nwh != m_state.m_nwh ||
                        (win.m_width != m_state.m_width || win.m_height != m_state.m_height))
                    {
                        // When window changes size or native window handle changed
                        // frame buffer must be recreated.
                        if (bgfx::isValid(m_fbh[viewId]))
                        {
                            bgfx::destroy(m_fbh[viewId]);
                            m_fbh[viewId].idx = bgfx::kInvalidHandle;
                        }

                        win.m_nwh = m_state.m_nwh;
                        win.m_width = m_state.m_width;
                        win.m_height = m_state.m_height;

                        if (NULL != win.m_nwh)
                        {
                            m_fbh[viewId] =
                                bgfx::createFrameBuffer(win.m_nwh, uint16_t(win.m_width), uint16_t(win.m_height));
                        }
                        else
                        {
                            win.m_handle.idx = UINT16_MAX;
                        }
                    }
                }
            }

            bgfx::touch(0);

            /*
            const bx::Vec3 at = {0.0f, 0.0f, 0.0f};
            const bx::Vec3 eye = {0.0f, 0.0f, -35.0f};

            float view[16];
            bx::mtxLookAt(view, eye, at);

            float proj[16];
            bx::mtxProj(proj, 60.0f, float(m_width) / float(m_height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);

            bgfx::setViewTransform(0, view, proj);
            bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));
            // This dummy draw call is here to make sure that view 0 is cleared
            // if no other draw calls are submitted to view 0.
            //bgfx::touch(0);

            // Set view and projection matrix for view 0.
            for (uint8_t ii = 1; ii < MAX_WINDOWS; ++ii)
            {
                bgfx::setViewTransform(ii, view, proj);
                bgfx::setViewFrameBuffer(ii, m_fbh[ii]);

                if (!bgfx::isValid(m_fbh[ii]))
                {
                    // Set view to default viewport.
                    bgfx::setViewRect(ii, 0, 0, uint16_t(m_width), uint16_t(m_height));
                    //bgfx::setViewClear(ii, BGFX_CLEAR_NONE);
                }
                else
                {
                    bgfx::setViewRect(ii, 0, 0, uint16_t(m_windows[ii].m_width), uint16_t(m_windows[ii].m_height));
                    //bgfx::setViewClear(ii, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
                }
            }

            int64_t now = bx::getHPCounter();
            float time = (float)((now - m_timeOffset) / double(bx::getHPFrequency()));

            if (NULL != m_bindings)
            {
                bgfx::dbgTextPrintf(0, 1, 0x2f, "Press 'c' to create or 'd' to destroy window.");
            }
            else
            {
                bool blink = uint32_t(time * 3.0f) & 1;
                bgfx::dbgTextPrintf(0, 0, blink ? 0x4f : 0x04, " Multiple windows is not supported by `%s` renderer. ",
                                    bgfx::getRendererName(bgfx::getCaps()->rendererType));
            }

            uint32_t count = 0;

            // Submit 11x11 cubes.
            for (uint32_t yy = 0; yy < 11; ++yy)
            {
                for (uint32_t xx = 0; xx < 11; ++xx)
                {
                    float mtx[16];
                    bx::mtxRotateXY(mtx, time + xx * 0.21f, time + yy * 0.37f);
                    mtx[12] = -15.0f + float(xx) * 3.0f;
                    mtx[13] = -15.0f + float(yy) * 3.0f;
                    mtx[14] = 0.0f;

                    // Set model matrix for rendering.
                    bgfx::setTransform(mtx);

                    // Set vertex and index buffer.
                    //bgfx::setVertexBuffer(0, m_vbh);
                    //bgfx::setIndexBuffer(m_ibh);

                    // Set render states.
                    bgfx::setState(BGFX_STATE_DEFAULT);

                    // Submit primitive for rendering.
                    //bgfx::submit(count % MAX_WINDOWS, m_program);
                    ++count;
                }
            }*/

            if (initiated)
            {
                bool runResult = _loopMain();
            }
            // Advance to next frame. Rendering thread will be kicked to
            // process submitted rendering primitives.
            //

            //bgfx::frame();

            


            return true;
        }
        width = ini->GetLong(nullptr, "screen_x", 1024);
        height = ini->GetLong(nullptr, "screen_y", 768);
        fullscreen = ini->GetLong(nullptr, "full_screen", false) ? true : false;

        return false;
    }

        virtual int shutdown() override
    {

        // Release
        core.ReleaseBase();
        ClipCursor(nullptr);

        imguiDestroy();

        for (uint32_t ii = 0; ii < MAX_WINDOWS; ++ii)
        {
            if (bgfx::isValid(m_fbh[ii]))
            {
                bgfx::destroy(m_fbh[ii]);
            }
        }

    std::shared_ptr<storm::OSWindow> window = storm::OSWindow::Create(width, height, fullscreen);
    window->SetTitle("Sea Dogs");
    core.Set_Hwnd(static_cast<HWND>(window->OSHandle()));
    window->Subscribe(HandleWindowEvent);
    window->Show();

    /* Init stuff */
    core.InitBase();

    /* Message loop */
    auto dwOldTime = GetTickCount();
        inputRemoveBindings("Sea Dogs");
        BX_FREE(entry::getAllocator(), m_bindings);

        // Cleanup.
        bgfx::destroy(m_ibh);
        bgfx::destroy(m_vbh);
        //bgfx::destroy(m_program);

        // Shutdown bgfx.
        bgfx::shutdown();

        return 0;
    }

    entry::WindowState m_state;

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_debug;
    uint32_t m_reset;

    bgfx::VertexBufferHandle m_vbh;
    bgfx::IndexBufferHandle m_ibh;
    bgfx::ProgramHandle m_program;

    entry::WindowState m_windows[MAX_WINDOWS];
    bgfx::FrameBufferHandle m_fbh[MAX_WINDOWS];

    InputBinding *m_bindings;

    int64_t m_timeOffset;
};

} // namespace

ENTRY_IMPLEMENT_MAIN(bgfx::ExampleWindows, "Sea Dogs", "Rendering test",
                     "https://bkaradzic.github.io/bgfx/examples.html#windows");

void cmdCreateWindow(const void *_userData)
{
    ((bgfx::ExampleWindows *)_userData)->createWindow();
    //core.Set_Hwnd((HWND)((ExampleWindows *)_userData)->m_windows[0].m_nwh);
}

void cmdDestroyWindow(const void *_userData)
{
    ((bgfx::ExampleWindows *)_userData)->destroyWindow();
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

