//
// Adventure
//

#pragma once

#include <Rk/string_ref.hpp>

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

    void show ();
    void pump ();
    void flip ();

    int width  () const;
    int height () const;

  };

}
