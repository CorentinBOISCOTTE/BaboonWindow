#include "baboon/win32_window.h"
#include "baboon/baboon_window.h"
#include <stdlib.h>
#include <windowsx.h>
#include <wingdi.h>
#include <stdbool.h>

static HINSTANCE g_hinstance = NULL;
static bool g_initialized = false;
static const wchar_t *g_class_name = L"BaboonWindowClass";
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

static int get_mods(void)
{
    int mods = 0;

    if (GetKeyState(VK_CONTROL) & 0x8000)
        mods |= BABOON_MOD_CONTROL;
    if (GetKeyState(VK_SHIFT) & 0x8000)
        mods |= BABOON_MOD_SHIFT;
    if (GetKeyState(VK_MENU) & 0x8000)
        mods |= BABOON_MOD_ALT;
    if (GetKeyState(VK_CAPITAL) & 1)
        mods |= BABOON_MOD_CAPS_LOCK;
    if (GetKeyState(VK_NUMLOCK) & 1)
        mods |= BABOON_MOD_NUM_LOCK;
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
        mods |= BABOON_MOD_SUPER;

    return mods;
}

static LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Win32Window *window = (Win32Window*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    switch (uMsg)
    {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        {
            int i, button, action;

            if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP)
                button = BABOON_MB_LEFT;
            else if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP)
                button = BABOON_MB_RIGHT;
            else if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP)
                button = BABOON_MB_MIDDLE;
            else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
                button = BABOON_MB_4;
            else
                button = BABOON_MB_5;

            if (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN || uMsg == WM_XBUTTONDOWN)
                action = BABOON_PRESS;
            else
                action = BABOON_RELEASE;

            for (i = 0; i <= BABOON_MB_LAST; ++i)
            {
                if (window->internal_window->mouse_buttons[i] == BABOON_PRESS)
                    break;
            }

            if (i > BABOON_MB_LAST)
                SetCapture(hwnd);

            baboon_input_mouse_click(window->internal_window, (void*)window, button, action, get_mods());

            for (i = 0; i <= BABOON_MB_LAST; ++i)
            {
                if (window->internal_window->mouse_buttons[i] == BABOON_PRESS)
                    break;
            }

            if (i > BABOON_MB_LAST)
                ReleaseCapture();

            if (uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONUP)
                return TRUE;

            return 0;
        }

        case WM_MOUSEMOVE:
        {
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);

            if (!window->internal_window->cursor_tracked)
            {
                TRACKMOUSEEVENT tme;
                ZeroMemory(&tme, sizeof(tme));
                tme.cbSize = sizeof(tme);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = window->hwnd;
                TrackMouseEvent(&tme);

                window->internal_window->cursor_tracked = BABOON_TRUE;
                baboon_input_mouse_enter(window->internal_window, (void*)window, BABOON_TRUE);
            }

            if (window->internal_window->cursor_mode == BABOON_CURSOR_DISABLED)
            {
                const int dx = x - window->internal_window->last_x_pos;
                const int dy = y - window->internal_window->last_y_pos;

                baboon_input_mouse_pos(window->internal_window, (void*)window, window->internal_window->virtual_x_pos + dx, window->internal_window->virtual_y_pos + dy);
            }
            else
                baboon_input_mouse_pos(window->internal_window, (void*)window, x, y);

            window->internal_window->last_x_pos = (double)x;
            window->internal_window->last_y_pos = (double)y;

            return 0;
        }

        case WM_MOUSELEAVE:
        {
            window->internal_window->cursor_tracked = BABOON_FALSE;
            baboon_input_mouse_enter(window->internal_window, (void*)window, BABOON_FALSE);
            return 0;
        }

        case WM_SIZE:
            if (window)
            {
                int width = LOWORD(lParam);
                int height = HIWORD(lParam);
                window->internal_window->width = width;
                window->internal_window->height = height;

                if (window->internal_window->callbacks.resize)
                    window->internal_window->callbacks.resize((BaboonWindow*)window, width, height);
            }
            return 0;

        case WM_CLOSE:
            if (window)
            {
                DestroyWindow(hwnd);
                window->internal_window->should_close = BABOON_TRUE;
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

    window->internal_window = initialize_internal_window();

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
    window->internal_window->width = width;
    window->internal_window->height = height;
    window->internal_window->should_close = false;

    SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)window);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    return (BaboonWindow*)window;
}

void set_resize_callback(BaboonWindow *window, baboon_resize_callback callback)
{
    if (!window || !callback)
        return;

    Win32Window* win32_window = (Win32Window*)window;

    win32_window->internal_window->callbacks.resize = callback;
}

void set_mouse_click_callback(BaboonWindow *window, baboon_mb_callback callback)
{
    if (!window || !callback)
        return;

    Win32Window* win32_window = (Win32Window*)window;

    win32_window->internal_window->callbacks.mouse_button = callback;
}

void set_mouse_pos_callback(BaboonWindow *window, baboon_mouse_pos_callback callback)
{
    if (!window || !callback)
        return;

    Win32Window* win32_window = (Win32Window*)window;

    win32_window->internal_window->callbacks.mouse_pos = callback;
}

void set_mouse_enter_callback(BaboonWindow *window, baboon_mouse_enter_callback callback)
{
    if (!window || !callback)
        return;

    Win32Window* win32_window = (Win32Window*)window;

    win32_window->internal_window->callbacks.mouse_enter = callback;
}

void set_cursor_pos(BaboonWindow *window, double x_pos, double y_pos)
{
    if (!window)
        return;

    Win32Window* win32_window = (Win32Window*)window;

    POINT pos = { (int)x_pos, (int)y_pos };

    win32_window->internal_window->last_x_pos = x_pos;
    win32_window->internal_window->last_y_pos = y_pos;

    ClientToScreen(win32_window->hwnd, &pos);
    SetCursorPos(pos.x, pos.y);
}

int get_mouse_button(BaboonWindow *window, int button)
{
    if (!window || button < 0 || button > BABOON_MB_LAST)
        return BABOON_NONE;

    Win32Window* win32_window = (Win32Window*)window;

    return win32_window->internal_window->mouse_buttons[button];
}

void get_cursor_pos(BaboonWindow *window, double *x_pos, double *y_pos)
{
    if (!window || !x_pos || !y_pos)
        return;

    Win32Window* win32_window = (Win32Window*)window;

    POINT cursor_pos;

    if (GetCursorPos(&cursor_pos))
    {
        ScreenToClient(win32_window->hwnd, &cursor_pos);

        *x_pos = cursor_pos.x;
        *y_pos = cursor_pos.y;
    }
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

void wait_events(void)
{
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void get_window_size(BaboonWindow *window, int *width, int *height)
{
    if (!window || !width || !height)
        return;

    Win32Window *win32_window = (Win32Window*)window;

    *width = win32_window->internal_window->width;
    *height = win32_window->internal_window->height;
}

BaboonBool window_should_close(BaboonWindow *window)
{
    if (!window)
        return true;
    
    Win32Window *win32_window = (Win32Window*)window;
    return win32_window->internal_window->should_close;
}

void set_window_should_close(BaboonWindow* window, BaboonBool should_close)
{
    if (!window)
        return;

    Win32Window* win32_window = (Win32Window*)window;
    win32_window->internal_window->should_close = should_close;
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

    free(win32_window->internal_window);
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

gl_proc baboon_get_proc_address(const char *proc_name)
{
    if (!proc_name)
        return NULL;

    gl_proc proc = (gl_proc)wglGetProcAddress(proc_name);

    if (!proc)
    {
        HMODULE opengl_module = GetModuleHandleA("opengl32.dll");
        if (opengl_module)
            proc = (gl_proc)GetProcAddress(opengl_module, proc_name);
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

void baboon_set_cursor_mode(BaboonWindow *window, int value)
{
    if (!window)
        return;

    Win32Window* win32_window = (Win32Window*)window;

    if (win32_window->hwnd == GetActiveWindow())
    {
        if (value == BABOON_CURSOR_DISABLED)
        {
            get_cursor_pos(window, &win32_window->internal_window->restore_cursor_pos_x, &win32_window->internal_window->restore_cursor_pos_y);
            center_cursor(window);
        }
        
        if (value == BABOON_CURSOR_DISABLED || value == BABOON_CURSOR_CAPTURED)
            capture_cursor(window);
        else
            release_cursor();

        if (value == BABOON_CURSOR_DISABLED)
            cursor_disabled_window = window;
        else if (cursor_disabled_window == window)
        {
            cursor_disabled_window = NULL;
            set_cursor_pos(window, win32_window->internal_window->restore_cursor_pos_x, win32_window->internal_window->restore_cursor_pos_y);
        }
    }
}

void capture_cursor(BaboonWindow *window)
{
    if (!window)
        return;

    Win32Window* win32_window = (Win32Window*)window;
    HWND handle = win32_window->hwnd;

    RECT clip_rect;

    GetClientRect(handle, &clip_rect);
    ClientToScreen(handle, (POINT*)&clip_rect.left);
    ClientToScreen(handle, (POINT*)&clip_rect.right);
    ClipCursor(&clip_rect);

    cursor_captured_window = window;
}

void release_cursor(void)
{
    ClipCursor(NULL);
    cursor_captured_window = NULL;
}

_BaboonWindow* get_internal_window(BaboonWindow *window)
{
    Win32Window* win32_window = (Win32Window*)window;

    return win32_window->internal_window;
}
