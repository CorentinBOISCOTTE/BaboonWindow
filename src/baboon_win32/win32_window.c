#include "baboon/win32_window.h"
#include "baboon/baboon_window.h"
#include <stdlib.h>
#include <windowsx.h>
#include <wingdi.h>

static HINSTANCE g_hinstance = NULL;
static bool g_initialized = false;
static const wchar_t *g_class_name = L"BaboonWindowClass";
static resize_callback g_resize_callback = NULL;
static HGLRC g_gl_context = NULL;

static LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static bool register_window_class(void)
{
    WNDCLASSW wnd_class = 
    {
        .style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS,
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
    Win32Window *window = (Win32Window*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    switch (uMsg)
    {
    case WM_SIZE:
        if (window)
        {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            window->width = width;
            window->height = height;

            if (g_resize_callback)
                g_resize_callback((BaboonWindow*)window, width, height);
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

BaboonWindow* create_window(int width, int height, const char *title)
{
    if (!ensure_initialized())
        return NULL;

    Win32Window *window = (Win32Window*)malloc(sizeof(Win32Window));
    if (!window)
        return NULL;

    size_t new_size = strlen(title) + 1;
    wchar_t *w_title = (wchar_t*)malloc(new_size * sizeof(wchar_t));
    size_t converted_chars = 0;
    mbstowcs_s(&converted_chars, w_title, new_size, title, _TRUNCATE);
    if (!w_title)
    {
        free(window);
        return NULL;
    }

    RECT rect = { 0, 0, width, height };
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);
    int window_width = rect.right - rect.left;
    int window_height = rect.bottom - rect.top;

    HWND hwnd = CreateWindowExW(
        0,
        g_class_name,
        w_title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        window_width,
        window_height,
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

void set_resize_callback(resize_callback callback)
{
    g_resize_callback = callback;
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

void get_window_size(BaboonWindow *window, int *width, int *height)
{
    if (!window || !width || !height)
        return;

    Win32Window *win32_window = (Win32Window*)window;

    *width = win32_window->width;
    *height = win32_window->height;
}

bool window_should_close(BaboonWindow *window)
{
    if (!window)
        return true;
    
    Win32Window *win32_window = (Win32Window*)window;
    return win32_window->should_close;
}

void destroy_window(BaboonWindow *window)
{
    if (!window)
        return;

    Win32Window *win32_window = (Win32Window*)window;

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

void swap_buffers(BaboonWindow *window)
{
    if (!window)
        return;

    Win32Window *win32_window = (Win32Window*)window;
    SwapBuffers(win32_window->hdc);
}

void make_context_current(BaboonWindow *window)
{
    Win32Window *win32_window = (Win32Window*)window;

    HDC hdc = win32_window->hdc;

    PIXELFORMATDESCRIPTOR pfd = 
    {
        .nSize = sizeof(PIXELFORMATDESCRIPTOR),
        .nVersion = 1,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .iPixelType = PFD_TYPE_RGBA,
        .cColorBits = 32,
        .cDepthBits = 24,
        .iLayerType = PFD_MAIN_PLANE,
    };

    int pixel_format = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixel_format, &pfd);

    g_gl_context = wglCreateContext(hdc);
    wglMakeCurrent(hdc, g_gl_context);
}

void baboon_terminate(void)
{
    if (g_gl_context)
    {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(g_gl_context);
    }
}

GLProc baboon_get_proc_adress(const char *proc_name)
{
    if (!proc_name)
        return NULL;

    GLProc proc = (GLProc)wglGetProcAddress(proc_name);

    if (!proc)
    {
        HMODULE opengl_module = GetModuleHandleA("opengl32.dll");
        if (opengl_module)
            proc = (GLProc)GetProcAddress(opengl_module, proc_name);
    }

    return proc;
}

HWND get_win32_window(BaboonWindow *window)
{
    if (!window)
        return NULL;

    Win32Window *win32_window = (Win32Window*)window;
    return win32_window->hwnd;
}
