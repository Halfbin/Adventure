//
// Adventure
//

#include "InputSystem.hpp"

#include <Rk/exception.hpp>

#include "MinWin.hpp"

namespace Ad
{
  namespace
  {
    using namespace Keys;

    const Key inv = Keys::invalid;

    const Key keytab [256] = {
      inv,      inv,      inv,      inv,      inv,      inv,      inv,      inv,
      bk_space, tab,      inv,      inv,      inv,      enter,    inv,      inv,
      inv,      inv,      inv,      pause,    capslock, inv,      inv,      inv,
      inv,      inv,      inv,      escape,   inv,      inv,      inv,      inv,
      spacebar, page_up,  page_dn,  end,      home,     left,     up,       right,
      down,     inv,      inv,      inv,      prt_scrn, insert,   del,      inv,
      top_0,    top_1,    top_2,    top_3,    top_4,    top_5,    top_6,    top_7,
      top_8,    top_9,    inv,      inv,      inv,      inv,      inv,      inv,
      inv,      alpha_a,  alpha_b,  alpha_c,  alpha_d,  alpha_e,  alpha_f,  alpha_g,
      alpha_h,  alpha_i,  alpha_j,  alpha_k,  alpha_l,  alpha_m,  alpha_n,  alpha_o,
      alpha_p,  alpha_q,  alpha_r,  alpha_s,  alpha_t,  alpha_u,  alpha_v,  alpha_w,
      alpha_x,  alpha_y,  alpha_z,  inv,      inv,      inv,      inv,      inv,
      np_0,     np_1,     np_2,     np_3,     np_4,     np_5,     np_6,     np_7,
      np_8,     np_9,     np_mul,   np_plus,  np_enter, np_minus, np_point, np_div,
      f1,       f2,       f3,       f4,       f5,       f6,       f7,       f8,
      f9,       f10,      f11,      f12,      f13,      f14,      f15,      f16,
      f17,      f18,      f19,      f20,      f21,      f22,      f23,      f24,
      inv,      inv,      inv,      inv,      inv,      inv,      inv,      inv,
      num_lock, scr_lock, inv,      inv,      inv,      inv,      inv,      inv,
      inv,      inv,      inv,      inv,      inv,      inv,      inv,      inv,
      lt_shift, rt_shift, lt_ctrl,  rt_ctrl,  lt_alt,   rt_alt,   inv,      inv,
      inv,      inv,      inv,      inv,      inv,      inv,      inv,      inv,
      inv,      inv,      inv,      inv,      inv,      inv,      inv,      inv,
      inv,      inv,      semi_col, equals,   comma,    dash,     period,   slash,
      backtick, inv,      inv,      inv,      inv,      inv,      inv,      inv,
      inv,      inv,      inv,      inv,      inv,      inv,      inv,      inv,
      inv,      inv,      inv,      inv,      inv,      inv,      inv,      inv,
      inv,      inv,      inv,      lt_brace, bk_slash, rt_brace, apos,     hash,
      inv,      inv,      inv,      inv,      inv,      inv,      inv,      inv,
      inv,      inv,      inv,      inv,      inv,      inv,      inv,      inv,
      inv,      inv,      inv,      inv,      inv,      inv,      inv,      inv,
      inv,      inv,      inv,      inv,      inv,      inv,      inv,      inv
    };

    Key vk_to_key (u8 vkey)
    {
      return keytab [vkey];
    }

  } // local

  void InputSystem::begin_update ()
  {
    pointer = { 0, 0 };
    wheel_delta = 0.0f;
    events.clear ();
    for (auto& ks : keys)
      ks.changed = false;
  }

  InputContext InputSystem::end_update (Frontend& fe)
  {
    auto ev_bg = events.data (),
         ev_ed = ev_bg + events.size ();

    InputContext ctx { fe };
    ctx.events = { ev_bg, ev_ed };
    ctx.keys = keys;
    ctx.buttons = buttons;
    ctx.pointer = pointer;
    ctx.wheel   = wheel_delta;
    return ctx;
  }

  iptr InputSystem::handle (uptr src, uint msg, uptr wp, iptr lp)
  {
    switch (msg)
    {
      case WM_SIZE:
        if (mouse_looking)
        {
          POINT area [2] { { 0, 0 }, { } };
          ClientToScreen ((HWND) src, area);
          area [1].x = area [0].x + LOWORD (lp);
          area [1].y = area [0].y + HIWORD (lp);
          ClipCursor ((const RECT*) area);
        }
      return 0;

      case WM_SETFOCUS:
        get_focus ((uptr) src);
      return 0;

      case WM_KILLFOCUS:
        lose_focus ((uptr) src);
      return 0;

      case WM_LBUTTONDOWN: events.push_back (button_event (Buttons::left,   true )); return 0;
      case WM_LBUTTONUP:   events.push_back (button_event (Buttons::left,   false)); return 0;
      case WM_MBUTTONDOWN: events.push_back (button_event (Buttons::middle, true )); return 0;
      case WM_MBUTTONUP:   events.push_back (button_event (Buttons::middle, false)); return 0;
      case WM_RBUTTONDOWN: events.push_back (button_event (Buttons::right,  true )); return 0;
      case WM_RBUTTONUP:   events.push_back (button_event (Buttons::right,  false)); return 0;

      case WM_INPUT:
        if (!focus || wp == RIM_INPUTSINK)
          break;

        on_raw_input (lp);
      return 0;

      default:;
    }

    return DefWindowProcW ((HWND) src, msg, wp, lp);
  }

  void InputSystem::on_mouse (const RAWMOUSE& rm)
  {
    ushort button_state = rm.usButtonFlags;

    ushort down_flag = 1,
           up_flag   = 2;

    for (int i = 0; i != 5; i++)
    {
      if (button_state & down_flag)
      {
        events.push_back (button_event (Button (i), true));
        buttons [i] = true;
      }
      else if (button_state & up_flag)
      {
        events.push_back (button_event (Button (i), false));
        buttons [i] = false;
      }

      down_flag <<= 2;
      up_flag   <<= 2;
    }

    if (button_state & 0x400)
    {
      wheel_delta = i16 (rm.usButtonData) * 1.0f;
      events.push_back (wheel_event (wheel_delta));
    }

    v2i pointer_instant = { rm.lLastX, rm.lLastY };

    if (rm.usFlags & MOUSE_MOVE_ABSOLUTE)
    {
      pointer_instant -= pointer_prev;
      pointer_prev = { rm.lLastX, rm.lLastY };
    }

    events.push_back (pointer_event (pointer_instant));
    pointer += pointer_instant;
  }

  void InputSystem::on_key (const RAWKEYBOARD& rk)
  {
    auto vk = rk.VKey;
    auto scan_code = rk.MakeCode;

    if (vk == 255) // Escape sequence non-key; ignore
      return;

    if (vk == VK_SHIFT)
      vk = MapVirtualKey (scan_code, MAPVK_VSC_TO_VK_EX); // Left/Right differntiation
    else if (vk == VK_NUMLOCK)
      scan_code = MapVirtualKey (vk, MAPVK_VK_TO_VSC) | 0x100; // Pause/NumLock silliness

    auto e0 = (rk.Flags & RI_KEY_E0) != 0,
         e1 = (rk.Flags & RI_KEY_E1) != 0;

    if (e1)
    {
      if (vk == VK_PAUSE) // Work around MapVirtualKey bug
        scan_code = 0x45;
      else
        scan_code = MapVirtualKey (vk, MAPVK_VK_TO_VSC);
    }

    switch (vk)
    {
      case VK_CONTROL:
        if (e0) vk = VK_RCONTROL;
        else    vk = VK_LCONTROL;
      break;

      case VK_MENU:
        if (e0) vk = VK_RMENU;
        else    vk = VK_LMENU;
      break;

      case VK_RETURN:
        if (e0) vk = VK_SEPARATOR;
      break;

      case VK_DELETE: if (!e0) vk = VK_DECIMAL; break;
      case VK_INSERT: if (!e0) vk = VK_NUMPAD0; break;
      case VK_END:    if (!e0) vk = VK_NUMPAD1; break;
      case VK_DOWN:   if (!e0) vk = VK_NUMPAD2; break;
      case VK_NEXT:   if (!e0) vk = VK_NUMPAD3; break;
      case VK_LEFT:   if (!e0) vk = VK_NUMPAD4; break;
      case VK_CLEAR:  if (!e0) vk = VK_NUMPAD5; break;
      case VK_RIGHT:  if (!e0) vk = VK_NUMPAD6; break;
      case VK_HOME:   if (!e0) vk = VK_NUMPAD7; break;
      case VK_UP:     if (!e0) vk = VK_NUMPAD8; break;
      case VK_PRIOR:  if (!e0) vk = VK_NUMPAD9; break;

      default:;
    }

    auto down = (rk.Flags & RI_KEY_BREAK) == 0;
    auto key = vk_to_key (vk & 0xff);

    if (down == keys [key].down)
      return;

    events.push_back (key_event (key, down));
    keys [key] = KeyState { down, true };

    // getting a human-readable string
  /*if (!down)
      return;

    auto lp = (scan_code << 16) | (e0 << 24);
    char16 buffer [64] = { };
    auto len = GetKeyNameTextW (lp, (wchar*) buffer, 63);
    buffer [len] = '\n';
    OutputDebugStringW ((wchar*) buffer);*/
  }

  void InputSystem::on_raw_input (iptr lp)
  {
    RAWINPUT input = { };
    uint input_len = sizeof (input);
    auto result = GetRawInputData ((HRAWINPUT) lp, RID_INPUT, &input, &input_len, sizeof (RAWINPUTHEADER));
    if (result == -1)
      return;

    // Shouldn't get anything we didn't subscribe to anyway
    if (input.header.dwType == RIM_TYPEMOUSE)
      on_mouse (input.data.mouse);
    else if (input.header.dwType == RIM_TYPEKEYBOARD)
      on_key (input.data.keyboard);
  }

  void InputSystem::enter_mouse_look (uptr target)
  {
    if (mouse_looking)
      return;

    RAWINPUTDEVICE mouse { 0x01, 0x02, RIDEV_NOLEGACY, (HWND) target };
    auto ok = RegisterRawInputDevices (&mouse, 1, sizeof (mouse));
    if (!ok)
      throw Rk::win_error ("RegisterRawInputDevices failed");

    while (ShowCursor (false) >= 0);
    
    RECT area;
    GetClientRect ((HWND) target, &area);
    ClipCursor (&area);

    mouse_looking = true;
  //OutputDebugStringA ("enter_mouse_look\n");
  }

  void InputSystem::leave_mouse_look (uptr target)
  {
    if (!mouse_looking)
      return;

    RAWINPUTDEVICE mouse { 0x01, 0x02, RIDEV_REMOVE, nullptr };
    RegisterRawInputDevices (&mouse, 1, sizeof (mouse));

    while (ShowCursor (true) < 0);

    ClipCursor (nullptr);

    mouse_looking = false;
  //OutputDebugStringA ("leave_mouse_look\n");
  }

  void InputSystem::get_focus (uptr target)
  {
    RAWINPUTDEVICE keyboard { 0x01, 0x06, RIDEV_APPKEYS | RIDEV_NOLEGACY, (HWND) target };
    auto ok = RegisterRawInputDevices (&keyboard, 1, sizeof (keyboard));
    if (!ok)
      throw Rk::win_error ("RegisterRawInputDevices failed");

    focus = true;
  }

  void InputSystem::lose_focus (uptr target)
  {
    leave_mouse_look (target);

    RAWINPUTDEVICE keyboard { 0x01, 0x06, RIDEV_REMOVE, nullptr };
    RegisterRawInputDevices (&keyboard, 1, sizeof (keyboard));

    focus = false;
  }

  InputSystem::InputSystem () :
    mouse_looking (false),
    focus         (false),
    pointer      { 0, 0 },
    pointer_prev { 0, 0 }
  { }

}
