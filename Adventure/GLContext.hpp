//
// Adventure
//

#pragma once

#include <Rk/types.hpp>

namespace Ad
{
  class GLContext
  {
    struct Impl;
    Impl* impl;

  public:
    GLContext (uptr target_handle);
    ~GLContext ();

    void flip ();

  };

}
