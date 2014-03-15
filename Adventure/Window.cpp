//
// Adventure
//

#include "Window.hpp"

#include "GLError.hpp"

#include <Rk/unicode_convert.hpp>
#include <Rk/exception.hpp>
#include <Rk/guard.hpp>

#include <GL/glew.h>
#include <GL/wglew.h>

#include "MinWin.hpp"

namespace Ad
{
  struct Window::Impl
  {
    HWND    window;
    HDC     dc;
    HGLRC   rc;
    std::function <void ()> on_close;
    
    int width,
        height;

    static LRESULT __stdcall window_procedure (HWND, UINT, WPARAM, LPARAM);
    LRESULT wndproc (HWND, UINT, WPARAM, LPARAM);

    Impl (Rk::cstring_ref title);
    ~Impl ();

  };

  LRESULT Window::Impl::wndproc (HWND src, UINT msg, WPARAM wp, LPARAM lp)
  {
    switch (msg)
    {
      case WM_CLOSE:
        on_close ();
      return 0;

      case WM_SIZE:
        width  = LOWORD (lp);
        height = HIWORD (lp);
      return 0;

      default:;
    }

    return DefWindowProcW (src, msg, wp, lp);
  }

  LRESULT __stdcall Window::Impl::window_procedure (HWND window, UINT msg, WPARAM wp, LPARAM lp)
  {
    auto self = reinterpret_cast <Impl*> (GetWindowLongPtr (window, GWLP_USERDATA));

    // Retrieve this from fake menu
    if (!self)
    {
      auto menu = GetMenu (window);

      MENUITEMINFO info = { };
      info.cbSize = sizeof (info);
      info.fMask = MIIM_DATA;
      GetMenuItemInfo (menu, 1, false, &info);

      self = reinterpret_cast <Impl*> (info.dwItemData);
      self -> window = window;

      SetWindowLongPtr (window, GWLP_USERDATA, reinterpret_cast <LONG_PTR> (self));
      SetMenu (window, nullptr);
    }

    return self -> wndproc (window, msg, wp, lp);
  }

  namespace
  {
    HWND create_dummy_window ()
    {
      // 1. Register class for dummy extension-grabbing window
      WNDCLASSEXW dummy_class = { };
      dummy_class.cbSize        = sizeof (dummy_class);
      dummy_class.hInstance     = GetModuleHandleW (0);
      dummy_class.lpfnWndProc   = &DefWindowProcW;
      dummy_class.lpszClassName = L"Ad::Window::Impl dummy";
      RegisterClassExW (&dummy_class);

      // 2. Create dummy window
      auto dummy_window = CreateWindowExW (
        0,
        dummy_class.lpszClassName,
        dummy_class.lpszClassName,
        0,
        100, 100,
        100, 100,
        0, 0,
        GetModuleHandleW (0),
        0
      );

      if (!dummy_window || iptr (dummy_window) == -1)
        throw Rk::win_error ("CreateWindowExW failed for dummy");

      return dummy_window;
    }

    HGLRC bind_dummy_context (HDC dummy_dc)
    {
      // Configure dummy window pixel format
      PIXELFORMATDESCRIPTOR dummy_pfd = { 0 };
      dummy_pfd.nSize      = sizeof (dummy_pfd);
      dummy_pfd.nVersion   = 1;
      dummy_pfd.dwFlags    = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
      dummy_pfd.iPixelType = PFD_TYPE_RGBA;
      dummy_pfd.cColorBits = 24;
      dummy_pfd.cDepthBits = 16;
      dummy_pfd.iLayerType = PFD_MAIN_PLANE;

      int format = ChoosePixelFormat (dummy_dc, &dummy_pfd);
      if (!format)
        throw Rk::win_error ("ChoosePixelFormat failed for dummy");

      i32 ok = SetPixelFormat (dummy_dc, format, &dummy_pfd);
      if (!ok)
        throw Rk::win_error ("SetPixelFormat failed for dummy");

      // Create and bind dummy context
      auto dummy_rc = wglCreateContext (dummy_dc);
      if (!dummy_rc)
        throw std::runtime_error ("wglCreateContext failed for dummy");

      auto dummy_current_ok = wglMakeCurrent (dummy_dc, dummy_rc);
      if (!dummy_current_ok)
      {
        wglDeleteContext (dummy_rc);
        throw std::runtime_error ("wglMakeCurrent failed for dummy");
      }

      return dummy_rc;
    }

    HWND create_display_window (WNDPROC wnd_proc, Rk::cstring_ref title, void* self)
    {
      // 7. Do-over with real display
      WNDCLASSEXW window_class = { };
      window_class.cbSize        = sizeof (window_class);
      window_class.hInstance     = GetModuleHandleW (0);
      window_class.hCursor       = LoadCursorW (window_class.hInstance, MAKEINTRESOURCE (IDC_ARROW));
      window_class.hIcon         = LoadIconW   (window_class.hInstance, MAKEINTRESOURCE (IDI_APPLICATION));
      window_class.lpfnWndProc   = wnd_proc;
      window_class.lpszClassName = L"Ad::Window::Impl";
      RegisterClassExW (&window_class);

      auto fake_menu = CreateMenu ();
      AppendMenuW (fake_menu, MF_OWNERDRAW, 1, (LPCWSTR) self);

      auto w_title = Rk::string_utf8_to_16 (title);

      auto window = CreateWindowExW (
        0,
        window_class.lpszClassName,
        (LPCWSTR) w_title.c_str (),
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        CW_USEDEFAULT, 0,
        800, 600,
        0,
        fake_menu,
        window_class.hInstance,
        0
      );

      if (!window || iptr (window) == -1)
        throw Rk::win_error ("CreateWindowExW for display");

      return window;
    }

    struct NewGLAPIs
    {
      PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormat;
      PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribs;
    };

    HGLRC bind_context (HDC dc, const NewGLAPIs& apis)
    {
      const i32 pixel_format_int_attribs [] = {
        WGL_DRAW_TO_WINDOW_ARB, true,
        WGL_SUPPORT_OPENGL_ARB, true,
        WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB,     24,
        WGL_ALPHA_BITS_ARB,     8,
        WGL_DEPTH_BITS_ARB,     16,
        WGL_DOUBLE_BUFFER_ARB,  true,
        WGL_SAMPLE_BUFFERS_ARB, true,
        WGL_SAMPLES_ARB,        4,
        0, 0
      };

      u32 format_count;
      int format;
      auto ok = apis.wglChoosePixelFormat (dc, pixel_format_int_attribs, nullptr, 1, &format, &format_count);
      if (!ok || format_count == 0)
        throw std::runtime_error ("wglChoosePixelFormat failed");

      PIXELFORMATDESCRIPTOR dummy_pfd = { };
      ok = SetPixelFormat (dc, format, &dummy_pfd); // PFD pointer is irrelevant, but necessary.
      if (!ok)
        throw Rk::win_error ("SetPixelFormat failed for display");

      const int context_attribs [] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
      #ifndef NDEBUG
        WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_DEBUG_BIT_ARB,
      #endif
        0, 0
      };

      auto rc = apis.wglCreateContextAttribs (dc, 0, context_attribs);
      if (!rc)
        throw std::runtime_error ("wglCreateContextAttribsARB failed");

      ok = wglMakeCurrent (dc, rc);
      if (!ok)
      {
        wglDeleteContext (rc);
        throw std::runtime_error ("wglMakeCurrent failed for display");
      }

      return rc;
    }

    template <typename Fn>
    void link_wgl (Fn& fn, const char* name)
    {
      fn = (Fn) wglGetProcAddress (name);
      if (!fn)
        throw std::runtime_error ("wglGetProcAddress failed");
    }

  } // local

  Window::Impl::Impl (Rk::cstring_ref title)
  {
    // Dummy window for grabbing new WGL apis
    auto dummy_window = create_dummy_window ();
    auto dummy_window_guard = Rk::guard ([=] { DestroyWindow (dummy_window); });

    // Dummy context
    auto dummy_dc = GetDC (dummy_window);
    if (!dummy_dc)
      throw Rk::win_error ("GetDC failed for dummy");
    auto dummy_dc_guard = Rk::guard ([=] { ReleaseDC (dummy_window, dummy_dc); });

    auto dummy_rc = bind_dummy_context (dummy_dc);
    auto dummy_rc_guard = Rk::guard ([=] { wglDeleteContext (dummy_rc); });

    // Grab APIs
    NewGLAPIs apis;
    link_wgl (apis.wglChoosePixelFormat,    "wglChoosePixelFormatARB");
    link_wgl (apis.wglCreateContextAttribs, "wglCreateContextAttribsARB");

    // Real display window
    window = create_display_window (&Impl::window_procedure, title, this);
    auto window_guard = Rk::guard ([=] { DestroyWindow (window); });

    // Real context
    dc = GetDC (window);
    if (!dc)
      throw Rk::win_error ("GetDC failed for display");
    auto dc_guard = Rk::guard ([=] { ReleaseDC (dummy_window, dummy_dc); });

    rc = bind_context (dc, apis);
    auto rc_guard = Rk::guard ([=] { wglDeleteContext (rc); });
    dummy_dc_guard.relieve (); // dummy_dc already killed by wglMakeCurrent in bind_context

    // Initialize GLEW.
    glewExperimental = true;
    auto glew_ok = glewInit ();
    if (glew_ok != GLEW_OK)
      throw std::runtime_error ("glewInit failed");
    glGetError (); // Ignore INVALID_ENUM sometimes generated by glewInit

    if (wglewIsSupported ("WGL_EXT_swap_control"))
    {
      PFNWGLSWAPINTERVALEXTPROC wglSwapInterval;
      link_wgl (wglSwapInterval, "wglSwapIntervalEXT");
      wglSwapInterval (0);
    }

    glEnable (GL_MULTISAMPLE);
    check_gl ("glEnable (GL_MULTISAMPLE) failed");

    // Done
    window_guard.relieve ();
    dc_guard.relieve ();
    rc_guard.relieve ();
  }

  Window::Impl::~Impl ()
  {
    wglDeleteContext (rc);
    ReleaseDC (window, dc);
    DestroyWindow (window);
  }

  void Window::on_close (std::function <void ()> func)
  {
    impl -> on_close = func;
  }

  void Window::show ()
  {
    ShowWindow (impl -> window, SW_SHOWNORMAL);
  }

  void Window::pump ()
  {
    MSG msg;
    while (PeekMessageW (&msg, nullptr, 0, 0, PM_REMOVE))
      DispatchMessageW (&msg);
  }

  void Window::flip ()
  {
    SwapBuffers (impl -> dc);
  }

  int Window::width () const
  {
    return impl -> width;
  }

  int Window::height () const
  {
    return impl -> height;
  }

  Window::Window (Rk::cstring_ref title) :
    impl (new Impl (title))
  { }

  Window::~Window ()
  {
    delete impl;
  }

}
