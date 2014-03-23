//
// Adventure
//

#include "Window.hpp"

#include "Input.hpp"

#include <Rk/unicode_convert.hpp>
#include <Rk/exception.hpp>
#include <Rk/guard.hpp>

#include "MinWin.hpp"

namespace Ad
{
  struct Window::Impl
  {
    HWND         window;
    InputSystem& in_sys;

    std::function <void ()> on_close;

    int width,
        height;

    static LRESULT __stdcall window_procedure (HWND, UINT, WPARAM, LPARAM);
    LRESULT wndproc (HWND, UINT, WPARAM, LPARAM);

    Impl (Rk::cstring_ref title, InputSystem&);
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

    return in_sys.handle ((uptr) src, msg, wp, lp);
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

  Window::Impl::Impl (Rk::cstring_ref title, InputSystem& in_sys) :
    in_sys (in_sys)
  {
    // Real display window
    WNDCLASSEXW window_class = { };
    window_class.cbSize        = sizeof (window_class);
    window_class.hInstance     = GetModuleHandleW (nullptr);
    window_class.hCursor       = LoadCursorW (nullptr, IDC_ARROW);
    window_class.hIcon         = LoadIconW   (nullptr, IDI_APPLICATION);
    window_class.lpfnWndProc   = window_procedure;
    window_class.lpszClassName = L"Ad::Window::Impl";
    RegisterClassExW (&window_class);

    // Neat way to get this ptr into first wndproc call
    auto fake_menu = CreateMenu ();
    AppendMenuW (fake_menu, MF_OWNERDRAW, 1, (LPCWSTR) this);

    auto w_title = Rk::string_utf8_to_16 (title);

    window = CreateWindowExW (
      0,
      window_class.lpszClassName,
      (LPCWSTR) w_title.c_str (),
      WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
      CW_USEDEFAULT, 0,
      1680, 1050,
      0,
      fake_menu,
      window_class.hInstance,
      0
    );

    if (!window || iptr (window) == -1)
      throw Rk::win_error ("CreateWindowExW for display");
  }

  Window::Impl::~Impl ()
  {
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

  InputContext Window::pump (Frontend& fe)
  {
    impl -> in_sys.begin_update ();

    MSG msg;
    while (PeekMessageW (&msg, nullptr, 0, 0, PM_REMOVE))
      DispatchMessageW (&msg);

    return impl -> in_sys.end_update (fe);
  }

  int Window::width () const
  {
    return impl -> width;
  }

  int Window::height () const
  {
    return impl -> height;
  }

  uptr Window::handle () const
  {
    return reinterpret_cast <uptr> (impl -> window);
  }

  Window::Window (Rk::cstring_ref title, InputSystem& in_sys) :
    impl (new Impl (title, in_sys))
  { }

  Window::~Window ()
  {
    delete impl;
  }

}
