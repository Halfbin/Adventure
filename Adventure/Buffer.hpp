//
// Adventure
//

#pragma once

#include "GLError.hpp"

#include <Rk/types.hpp>

#include <GL/glew.h>

namespace Ad
{
  class Buffer
  {
    uint glname;

  public:
    Buffer (size_t reserve)
    {
      glGenBuffers (1, &glname);
      check_gl ("glGenBuffers failed");

      glBindBuffer (GL_ARRAY_BUFFER, glname);
      check_gl ("glBindBuffer failed");

      glBufferData (GL_ARRAY_BUFFER, reserve, nullptr, GL_STREAM_DRAW);
      check_gl ("glBufferData failed");
    }

    ~Buffer ()
    {
      glDeleteBuffers (1, &glname);
    }

    uint name () const
    {
      return glname;
    }

  };

}
