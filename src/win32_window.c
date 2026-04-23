#include "win32_window.h"
#include "baboon_window.h"
#include <stdlib.h>

static HINSTANCE g_hinstance = NULL;
static bool g_initialized = false;
static const wchar_t* g_class_name = L"BaboonWindowClass";

static LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static bool register_window_class(void)
{
    WNDCLASSW wnd_class = 
    {
        .style = CS_VREDRAW | CS_HREDRAW,
        .lpfnWndProc = window_proc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = g_hinstance,
        .hIcon = LoadIconA(NULL, IDI_APPLICATION),
        .hCursor = LoadCursorA(NULL, IDC_ARROW),
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
        .lpszMenuName = NULL,
        .lpszClassName = g_class_name,
    };

    return RegisterClassW(&wnd_class) != 0;
}

static LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Win32Window* window = (Win32Window*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    switch (uMsg)
    {
    case WM_SIZE:
        if (window)
        {
            window->height = LOWORD(lParam);
            window->width = HIWORD(lParam);

            if (g_resize_callback)
                g_resize_callback((BaboonWindow*)window, window->width, window->height);
        }
        return 0;

    case WM_CLOSE:
        if (window)
        {
            DestroyWindow(hwnd);
            window->should_close = true;
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}

static bool ensure_initialized(void)
{
    if (g_initialized) 
        return true;

    g_hinstance = GetModuleHandleW(NULL);
    if (!g_hinstance) 
        return false;

    if (!register_window_class()) 
        return false;

    g_initialized = true;
    return true;
}

BaboonWindow* create_window(int width, int height, const char* title)
{
    if (!ensure_initialized())
        return NULL;

    Win32Window* window = (Win32Window*)malloc(sizeof(Win32Window));
    if (!window)
        return NULL;

    size_t new_size = strlen(title) + 1;
    wchar_t* w_title = (wchar_t*)malloc(new_size * sizeof(wchar_t));
    size_t converted_chars = 0;
    mbstowcs_s(&converted_chars, w_title, new_size, title, _TRUNCATE);
    if (!w_title)
    {
        free(window);
        return NULL;
    }

    HWND hwnd = CreateWindowExW(
        0,
        g_class_name,
        w_title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width,
        height,
        NULL,
        NULL,
        g_hinstance,
        NULL
    );

    free(w_title);

    if (!hwnd) {
        free(window);
        return NULL;
    }

    window->hwnd = hwnd;
    window->hinstance = g_hinstance;
    window->hdc = GetDC(hwnd);
    window->width = width;
    window->height = height;
    window->should_close = false;

    SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)window);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    return (BaboonWindow*)window;
}

void set_resize_callback(void(*resize_callback)(BaboonWindow*, int, int))
{
    g_resize_callback = resize_callback;
}

void poll_events(void)
{
    MSG msg;
    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void get_window_size(BaboonWindow* window, int* width, int* height)
{
    Win32Window* win32_window = (Win32Window*)window;

    *width = win32_window->width;
    *height = win32_window->height;
}

bool window_should_close(BaboonWindow* window)
{
    if (!window)
        return true;
    
    Win32Window* win32_window = (Win32Window*)window;
    return win32_window->should_close;
}

void destroy_window(BaboonWindow* window)
{
    if (!window)
        return;

    Win32Window* win32_window = (Win32Window*)window;

    if (win32_window->hdc) 
    {
        ReleaseDC(win32_window->hwnd, win32_window->hdc);
    }

    if (win32_window->hwnd) 
    {
        DestroyWindow(win32_window->hwnd);
    }

    free(win32_window);
}
