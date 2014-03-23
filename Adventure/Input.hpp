//
// Adventure
//

#pragma once

#include "Frontend.hpp"

#include <Rk/vector.hpp>

namespace Ad
{
  namespace Keys
  {
    enum Key
    {
      invalid = 0,

      alpha_a, alpha_b, alpha_c, alpha_d, alpha_e, alpha_f, alpha_g, alpha_h, alpha_i, alpha_j, alpha_k, alpha_l, alpha_m,
      alpha_n, alpha_o, alpha_p, alpha_q, alpha_r, alpha_s, alpha_t, alpha_u, alpha_v, alpha_w, alpha_x, alpha_y, alpha_z,

      top_0, top_1, top_2, top_3, top_4, top_5, top_6, top_7, top_8, top_9,

      backtick,
      dash,
      equals,
      bk_space,
      tab,
      lt_brace,
      rt_brace,
      enter,
      capslock,
      semi_col,
      apos,
      hash,
      lt_shift,
      bk_slash,
      comma,
      period,
      slash,
      rt_shift,
      lt_ctrl,
      lt_alt,
      spacebar,
      rt_alt,
      rt_ctrl,

      escape,
       f1,  f2,  f3,  f4,  f5,  f6,  f7,  f8,  f9, f10, f11, f12,
      f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24,

      prt_scrn,
      scr_lock,
      pause,

      insert,  del,
      home,    end,
      page_up, page_dn,

      up, down, left, right,

      num_lock,
      np_div,
      np_mul,
      np_minus,
      np_plus,
      np_point,
      np_enter,

      np_0, np_1, np_2, np_3, np_4, np_5, np_6, np_7, np_8, np_9,

      count
    };

  } // Keys

  using Keys::Key;

  struct KeyState
  {
    bool down,
         changed;

    explicit operator bool () const
    {
      return down;
    }

  };

  namespace Buttons
  {
    enum Button
    {
      left,
      middle,
      right,
      ex_4,
      ex_5,

      count
    };

  }

  using Buttons::Button;

  class ButtonState
  {
    int dn : 1;

  public:
    ButtonState () :
      dn (0)
    { }

    ButtonState (bool down) :
      dn (down)
    { }

    bool down () const { return dn != 0; }

    explicit operator bool () const
    {
      return down ();
    }

  };

  namespace Events
  {
    enum EventType
    {
      key,
      button,
      pointer
    };

  };

  using Events::EventType;

  class Event
  {
    EventType ty;
    u32 storage [4];

  public:
    template <typename E>
    Event (EventType new_ty, const E& data) :
      ty (new_ty)
    {
      new (storage) E (data);
    }

    bool is (EventType type) const
    {
      return ty == type;
    }

    EventType type () const
    {
      return ty;
    }

    template <typename E>
    const E& as () const
    {
      static_assert (sizeof (E) <= sizeof (storage), "Invalid event type");
      return *reinterpret_cast <const E*> (storage);
    }

  };

  struct KeyEvent
  {
    Key      key;
    KeyState state;
  };

  inline Event key_event (Key key, bool down)
  {
    return Event (Events::key, KeyEvent { key, KeyState { down, true } });
  }

  inline bool is_key_down (const Event& ev, Key k)
  {
    const auto& kev = ev.as <KeyEvent> ();
    return ev.is (Events::key) && kev.key == k && kev.state.down;
  }

  inline bool is_key_up (const Event& ev, Key k)
  {
    const auto& kev = ev.as <KeyEvent> ();
    return ev.is (Events::key) && kev.key == k && !kev.state.down;
  }

  struct ButtonEvent
  {
    Button button;
    bool   down;
  };

  inline Event button_event (Button button, bool down)
  {
    return Event (Events::button, ButtonEvent { button, down });
  }

  inline bool is_button_down (const Event& ev, Button b)
  {
    const auto& bev = ev.as <ButtonEvent> ();
    return ev.is (Events::button) && bev.button == b && bev.down;
  }

  inline bool is_button_up (const Event& ev, Button b)
  {
    const auto& bev = ev.as <ButtonEvent> ();
    return ev.is (Events::button) && bev.button == b && !bev.down;
  }

  struct PointerEvent
  {
    v2i delta;
  };

  inline Event pointer_event (v2i delta)
  {
    return Event (Events::pointer, PointerEvent { delta });
  }

}

namespace Rk
{
  template <typename begin_t, typename end_t = begin_t>
  class range
  {
    begin_t bg;
    end_t   ed;

  public:
    range () = default;

    range (begin_t begin, end_t end) : bg (begin), ed (end) { }

    begin_t begin () const { return bg; }
    end_t   end   () const { return ed; }

  };

  template <typename begin_t, typename end_t>
  inline begin_t begin (const range <begin_t, end_t>& range)
  {
    return range.begin ();
  }

  template <typename begin_t, typename end_t>
  inline end_t end (const range <begin_t, end_t>& range)
  {
    return range.end ();
  }

}

namespace Ad
{
  class InputContext
  {
  public:
    typedef Rk::range <const Event*> EventRange;

    InputContext (Frontend& fe) :
      frontend (fe),
      pointer  (nil)
    { }

    Frontend&          frontend;
    EventRange         events;
    const KeyState*    keys;
    const ButtonState* buttons;
    v2i                pointer;

  };

}
