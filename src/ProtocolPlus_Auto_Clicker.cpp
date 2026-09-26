#define UNICODE
#define _UNICODE

#include <windows.h>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <cstdlib>

static HWND g_cpsEdit;
static HWND g_buttonCombo;
static HWND g_hotkeyCombo;
static HWND g_status;
static HWND g_toggle;

static std::atomic<bool> g_running(false);
static std::atomic<bool> g_stop(false);

static const wchar_t* APP_NAME = L"Protocol+ Auto Clicker";

static void ClickLoop(double cps, DWORD downFlag, DWORD upFlag)
{
    if (cps < 1.0)
        cps = 1.0;

    if (cps > 1000.0)
        cps = 1000.0;

    const double interval = 1.0 / cps;

    auto nextClick = std::chrono::steady_clock::now();

    while (!g_stop.load())
    {
        INPUT inputs[2]{};

        inputs[0].type = INPUT_MOUSE;
        inputs[0].mi.dwFlags = downFlag;

        inputs[1].type = INPUT_MOUSE;
        inputs[1].mi.dwFlags = upFlag;

        SendInput(2, inputs, sizeof(INPUT));

        nextClick +=
            std::chrono::duration_cast<
                std::chrono::steady_clock::duration
            >(std::chrono::duration<double>(interval));

        auto now = std::chrono::steady_clock::now();

        if (nextClick > now)
        {
            std::this_thread::sleep_for(nextClick - now);
        }
        else
        {
            nextClick = now;
        }
    }
}

static double GetCPS()
{
    wchar_t buffer[32]{};

    GetWindowTextW(
        g_cpsEdit,
        buffer,
        31
    );

    double cps = _wtof(buffer);

    if (cps < 1.0 || cps > 1000.0)
        return 0.0;

    return cps;
}

static void SetStatus(const std::wstring& text)
{
    SetWindowTextW(
        g_status,
        text.c_str()
    );
}

static void ToggleClicker()
{
    if (g_running.load())
    {
        g_stop = true;
        g_running = false;

        SetStatus(L"Stopped");

        SetWindowTextW(
            g_toggle,
            L"Start"
        );

        return;
    }

    double cps = GetCPS();

    if (cps <= 0.0)
    {
        MessageBoxW(
            nullptr,
            L"Enter a CPS value from 1 to 1000.",
            APP_NAME,
            MB_ICONWARNING
        );

        return;
    }

    int button =
        static_cast<int>(
            SendMessageW(
                g_buttonCombo,
                CB_GETCURSEL,
                0,
                0
            )
        );

    DWORD downFlag = MOUSEEVENTF_LEFTDOWN;
    DWORD upFlag   = MOUSEEVENTF_LEFTUP;

    if (button == 1)
    {
        downFlag = MOUSEEVENTF_RIGHTDOWN;
        upFlag   = MOUSEEVENTF_RIGHTUP;
    }
    else if (button == 2)
    {
        downFlag = MOUSEEVENTF_MIDDLEDOWN;
        upFlag   = MOUSEEVENTF_MIDDLEUP;
    }

    g_stop = false;
    g_running = true;

    wchar_t status[64]{};

    swprintf_s(
        status,
        L"Running — %.0f CPS",
        cps
    );

    SetStatus(status);

    SetWindowTextW(
        g_toggle,
        L"Stop"
    );

    std::thread(
        ClickLoop,
        cps,
        downFlag,
        upFlag
    ).detach();
}

static void UpdateHotkey()
{
    int selection =
        static_cast<int>(
            SendMessageW(
                g_hotkeyCombo,
                CB_GETCURSEL,
                0,
                0
            )
        );

    UINT virtualKey = VK_F6 + selection;

    UnregisterHotKey(
        nullptr,
        1
    );

    RegisterHotKey(
        nullptr,
        1,
        0,
        virtualKey
    );
}

LRESULT CALLBACK WindowProcedure(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (message)
    {
        case WM_CREATE:
        {
            HFONT font =
                static_cast<HFONT>(
                    GetStockObject(DEFAULT_GUI_FONT)
                );

            HWND title =
                CreateWindowW(
                    L"STATIC",
                    L"Protocol+ Auto Clicker",
                    WS_CHILD | WS_VISIBLE,
                    24,
                    20,
                    350,
                    30,
                    hwnd,
                    nullptr,
                    nullptr,
                    nullptr
                );

            SendMessageW(
                title,
                WM_SETFONT,
                reinterpret_cast<WPARAM>(font),
                TRUE
            );

            CreateWindowW(
                L"STATIC",
                L"Clicks per second (CPS):",
                WS_CHILD | WS_VISIBLE,
                24,
                68,
                220,
                24,
                hwnd,
                nullptr,
                nullptr,
                nullptr
            );

            g_cpsEdit =
                CreateWindowExW(
                    WS_EX_CLIENTEDGE,
                    L"EDIT",
                    L"10",
                    WS_CHILD |
                    WS_VISIBLE |
                    ES_NUMBER,
                    270,
                    64,
                    100,
                    28,
                    hwnd,
                    nullptr,
                    nullptr,
                    nullptr
                );

            CreateWindowW(
                L"STATIC",
                L"Mouse button:",
                WS_CHILD | WS_VISIBLE,
                24,
                108,
                220,
                24,
                hwnd,
                nullptr,
                nullptr,
                nullptr
            );

            g_buttonCombo =
                CreateWindowW(
                    L"COMBOBOX",
                    L"",
                    WS_CHILD |
                    WS_VISIBLE |
                    CBS_DROPDOWNLIST,
                    270,
                    104,
                    100,
                    120,
                    hwnd,
                    nullptr,
                    nullptr,
                    nullptr
                );

            SendMessageW(
                g_buttonCombo,
                CB_ADDSTRING,
                0,
                reinterpret_cast<LPARAM>(L"Left")
            );

            SendMessageW(
                g_buttonCombo,
                CB_ADDSTRING,
                0,
                reinterpret_cast<LPARAM>(L"Right")
            );

            SendMessageW(
                g_buttonCombo,
                CB_ADDSTRING,
                0,
                reinterpret_cast<LPARAM>(L"Middle")
            );

            SendMessageW(
                g_buttonCombo,
                CB_SETCURSEL,
                0,
                0
            );

            CreateWindowW(
                L"STATIC",
                L"Toggle hotkey:",
                WS_CHILD | WS_VISIBLE,
                24,
                148,
                220,
                24,
                hwnd,
                nullptr,
                nullptr,
                nullptr
            );

            g_hotkeyCombo =
                CreateWindowW(
                    L"COMBOBOX",
                    L"",
                    WS_CHILD |
                    WS_VISIBLE |
                    CBS_DROPDOWNLIST,
                    270,
                    144,
                    100,
                    120,
                    hwnd,
                    nullptr,
                    nullptr,
                    nullptr
                );

            SendMessageW(
                g_hotkeyCombo,
                CB_ADDSTRING,
                0,
                reinterpret_cast<LPARAM>(L"F6")
            );

            SendMessageW(
                g_hotkeyCombo,
                CB_ADDSTRING,
                0,
                reinterpret_cast<LPARAM>(L"F7")
            );

            SendMessageW(
                g_hotkeyCombo,
                CB_ADDSTRING,
                0,
                reinterpret_cast<LPARAM>(L"F8")
            );

            SendMessageW(
                g_hotkeyCombo,
                CB_ADDSTRING,
                0,
                reinterpret_cast<LPARAM>(L"F9")
            );

            SendMessageW(
                g_hotkeyCombo,
                CB_SETCURSEL,
                0,
                0
            );

            g_status =
                CreateWindowW(
                    L"STATIC",
                    L"Stopped",
                    WS_CHILD |
                    WS_VISIBLE |
                    SS_CENTER,
                    24,
                    195,
                    346,
                    28,
                    hwnd,
                    nullptr,
                    nullptr,
                    nullptr
                );

            g_toggle =
                CreateWindowW(
                    L"BUTTON",
                    L"Start",
                    WS_CHILD |
                    WS_VISIBLE |
                    BS_PUSHBUTTON,
                    24,
                    235,
                    346,
                    42,
                    hwnd,
                    reinterpret_cast<HMENU>(1001),
                    nullptr,
                    nullptr
                );

            CreateWindowW(
                L"STATIC",
                L"Press the selected hotkey anywhere in Windows to toggle.",
                WS_CHILD |
                WS_VISIBLE |
                SS_CENTER,
                24,
                290,
                346,
                30,
                hwnd,
                nullptr,
                nullptr,
                nullptr
            );

            RegisterHotKey(
                nullptr,
                1,
                0,
                VK_F6
            );

            return 0;
        }

        case WM_COMMAND:
        {
            if (
                LOWORD(wParam) == 1001 &&
                HIWORD(wParam) == BN_CLICKED
            )
            {
                ToggleClicker();
            }

            if (
                reinterpret_cast<HWND>(lParam) == g_hotkeyCombo &&
                HIWORD(wParam) == CBN_SELCHANGE
            )
            {
                UpdateHotkey();
            }

            return 0;
        }

        case WM_HOTKEY:
        {
            if (wParam == 1)
            {
                ToggleClicker();
            }

            return 0;
        }

        case WM_DESTROY:
        {
            UnregisterHotKey(
                nullptr,
                1
            );

            g_stop = true;

            PostQuitMessage(0);

            return 0;
        }
    }

    return DefWindowProcW(
        hwnd,
        message,
        wParam,
        lParam
    );
}

int WINAPI wWinMain(
    HINSTANCE instance,
    HINSTANCE,
    PWSTR,
    int showCommand
)
{
    WNDCLASSW windowClass{};

    windowClass.lpfnWndProc =
        WindowProcedure;

    windowClass.hInstance =
        instance;

    windowClass.lpszClassName =
        L"ProtocolPlusAutoClicker";

    windowClass.hCursor =
        LoadCursorW(
            nullptr,
            IDC_ARROW
        );

    windowClass.hbrBackground =
        reinterpret_cast<HBRUSH>(
            COLOR_WINDOW + 1
        );

    RegisterClassW(
        &windowClass
    );

    HWND window =
        CreateWindowW(
            windowClass.lpszClassName,
            APP_NAME,
            WS_OVERLAPPED |
            WS_CAPTION |
            WS_SYSMENU |
            WS_MINIMIZEBOX,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            410,
            370,
            nullptr,
            nullptr,
            instance,
            nullptr
        );

    if (!window)
        return 1;

    ShowWindow(
        window,
        showCommand
    );

    UpdateWindow(window);

    MSG message{};

    while (
        GetMessageW(
            &message,
            nullptr,
            0,
            0
        ) > 0
    )
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return 0;
}
