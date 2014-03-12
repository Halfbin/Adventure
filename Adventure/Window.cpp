//
// Adventure
//

#include "Window.hpp"

#include <Rk/unicode_convert.hpp>
#include <Rk/exception.hpp>
#include <Rk/guard.hpp>

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

    return DefWindowProcW (window, msg, wp, lp);
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

  Window::Impl::Impl (Rk::cstring_ref title)
  {
    WNDCLASSEX window_class = { };
    window_class.cbSize        = sizeof (window_class);
    window_class.hInstance     = GetModuleHandleW (0);
    window_class.hCursor       = LoadCursorW (window_class.hInstance, MAKEINTRESOURCE (IDC_ARROW));
    window_class.hIcon         = LoadIconW   (window_class.hInstance, MAKEINTRESOURCE (IDI_APPLICATION));
    window_class.lpfnWndProc   = &Impl::window_procedure;
    window_class.lpszClassName = L"Ad::Window::Impl";

    RegisterClassExW (&window_class);

    auto fake_menu = CreateMenu ();
    AppendMenuW (fake_menu, MF_OWNERDRAW, 1, (LPCWSTR) this);

    auto w_title = Rk::string_utf8_to_16 (title);

    window = CreateWindowExW (
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

    if (!window || ~UINT_PTR (window) == 0)
      throw Rk::win_error ("Error creating display window");

    auto handle_guard = Rk::guard ([=] { DestroyWindow (window); });

    dc = GetDC (window);
    if (!dc)
      throw Rk::win_error ("Error retrieving display window DC");

    auto dc_guard = Rk::guard ([=] { ReleaseDC (window, dc); });

    PIXELFORMATDESCRIPTOR pfd = { };
    pfd.nSize = sizeof (pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.iLayerType = PFD_MAIN_PLANE;

    auto format_index = ChoosePixelFormat (dc, &pfd);
    if (!format_index)
      throw Rk::win_error ("Error choosing display pixel format");

    auto ok = SetPixelFormat (dc, format_index, &pfd);
    if (!ok)
      throw Rk::win_error ("Error setting display pixel format");

    rc = wglCreateContext (dc);
    if (!rc)
      throw Rk::win_error ("wglCreateContext failed");
    auto rc_guard = Rk::guard ([=] { wglDeleteContext (rc); });

    ok = wglMakeCurrent (dc, rc);
    if (!ok)
      throw Rk::win_error ("wglMakeCurrent failed");

    handle_guard.relieve ();
    dc_guard.relieve ();
    rc_guard.relieve ();
  }

  Window::Impl::~Impl ()
  {
    wglMakeCurrent (nullptr, nullptr);
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
