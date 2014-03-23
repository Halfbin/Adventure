//
// Adventure
//

#pragma once

#include "Input.hpp"

#include <vector>

typedef struct tagRAWMOUSE    RAWMOUSE;
typedef struct tagRAWKEYBOARD RAWKEYBOARD;

namespace Ad
{
  class InputSystem
  {
    bool                mouse_looking,
                        focus;
    std::vector <Event> events;
    v2i                 pointer,
                        pointer_prev;
    KeyState            keys    [Keys::count];
    ButtonState         buttons [Buttons::count];

    void get_focus  (uptr target);
    void lose_focus (uptr target);

  public:
    InputSystem ();

    iptr         handle           (uptr src, uint msg, uptr wp, iptr lp);
    void         begin_update     ();
    InputContext end_update       (Frontend& fe);
    void         on_raw_input     (iptr lp);
    void         on_mouse         (const RAWMOUSE&);
    void         on_key           (const RAWKEYBOARD&);
    void         enter_mouse_look (uptr target);
    void         leave_mouse_look (uptr target);

  };

}
