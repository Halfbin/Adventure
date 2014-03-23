//
// Adventure
//

#pragma once

#include <Rk/string_ref.hpp>

#include "InputSystem.hpp"
#include "Frontend.hpp"

#include <functional>

namespace Ad
{
  class Window
  {
    struct Impl;
    Impl* impl;

  public:
    Window (Rk::cstring_ref title, InputSystem& in_sys);
    ~Window ();

    void on_close (std::function <void ()> func);

    InputContext pump (Frontend&);

    void show ();

    int width  () const;
    int height () const;

    uptr handle () const;

  };

}
