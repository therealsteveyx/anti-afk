#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <atomic>

static std::atomic<bool> isRunning(false);

static void print(int r, int g, int b, const char* text) {
    std::cout << "\x1b[38;2;" << r << ";" << g << ";" << b << "m" << text << "\x1b[0m";
}

static void enableVT() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(h, &mode);
    SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

static void sendKey(WORD vk, int hold) {
    WORD scan = (WORD)MapVirtualKey(vk, MAPVK_VK_TO_VSC);
    INPUT down = {};
    down.type = INPUT_KEYBOARD;
    down.ki.wVk = vk;
    down.ki.wScan = scan;
    down.ki.dwFlags = KEYEVENTF_SCANCODE;

    INPUT up = down;
    up.ki.dwFlags |= KEYEVENTF_KEYUP;

    SendInput(1, &down, sizeof(INPUT));
    Sleep(hold);
    SendInput(1, &up, sizeof(INPUT));
}

static void sendRightClick(int hold) {
    INPUT down = {};
    down.type = INPUT_MOUSE;
    down.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

    INPUT up = down;
    up.mi.dwFlags = MOUSEEVENTF_RIGHTUP;

    SendInput(1, &down, sizeof(INPUT));
    Sleep(hold);
    SendInput(1, &up, sizeof(INPUT));
}

static void worker() {
    srand((unsigned)(time(nullptr) ^ GetCurrentThreadId()));
    const WORD keys[] = { 'W', 'D', 'A', 'S' };
    int idx = 0, tick = 0;

    for (;;) {
        if (!isRunning) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        if (tick > 0 && tick % 3 == 0) {
            sendRightClick(800 + rand() % 700);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        if (idx < 4) {
            sendKey(keys[idx++], 300 + rand() % 500);
        } else {
            sendKey('5', 300 + rand() % 500);
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
            sendKey('5', 300 + rand() % 500);
            idx = 0;
        }

        tick++;
        std::this_thread::sleep_for(std::chrono::milliseconds(400 + rand() % 800));
    }
}

static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
        auto* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

        if (kb->vkCode == VK_F1 && !isRunning) {
            isRunning = true;
            print(100, 255, 150, "  [ON] ");
            print(200, 200, 200, "Anti-AFK started\n");
        }
        else if (kb->vkCode == VK_F2 && isRunning) {
            isRunning = false;
            print(255, 100, 100, "  [OFF] ");
            print(200, 200, 200, "Anti-AFK stopped\n");
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main() {
    enableVT();
    SetConsoleTitleA("Anti-AFK");

    std::cout << "\n";
    print(255, 255, 255, "  ANTI-AFK");
    print(120, 120, 120, "  @therealsteveyx\n\n");

    print(255, 255, 255, "  F1");
    print(120, 120, 120, " - Start\n");
    print(255, 255, 255, "  F2");
    print(120, 120, 120, " - Stop\n\n");

    std::thread(worker).detach();

    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardProc, NULL, 0);
    if (!hook) {
        print(255, 100, 100, "  [ERROR] Hook failed. Run as admin.\n");
        system("pause");
        return 1;
    }

    print(100, 255, 150, "  [OK] ");
    print(150, 150, 150, "Ready\n\n");

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hook);
    return 0;
}
