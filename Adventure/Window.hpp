//
// Adventure
//

#pragma once

#include <Rk/string_ref.hpp>

#include "Input.hpp"

#include <functional>

namespace Ad
{
  class Window
  {
    struct Impl;
    Impl* impl;

  public:
    Window (Rk::cstring_ref title);
    ~Window ();

    void on_close (std::function <void ()> func);

    void pump ();

    void update_keys (KeyState* keys);

    const Event* events      () const;
    uint         event_count () const;

    void show ();
    void flip ();

    int width  () const;
    int height () const;

  };

}
