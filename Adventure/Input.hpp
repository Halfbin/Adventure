//
// Adventure
//

#pragma once

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
      left_alt,
      spacebar,
      right_alt,
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

      np_0, np_1, np_2, np_3, np_4, np_5, np_6, np_7, np_8, np_9,

      count
    };

  } // Keys

  using Keys::Key;

  class KeyState
  {
    int dn : 1,
        ch : 1,
        lk : 1;

  public:
    KeyState () :
      dn (0), ch (0), lk (0)
    { }

    KeyState (bool down, bool changed, bool locked) :
      dn (down),
      ch (changed),
      lk (locked)
    { }

    bool down    () const { return dn != 0; }
    bool changed () const { return ch != 0; }
    bool locked  () const { return lk != 0; }

    explicit operator bool () const
    {
      return down ();
    }

  };

  namespace Events
  {
    enum EventType
    {
      key_down,
      key_up
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
    Key key;
  };

}
