//
// Adventure
//

#pragma once

#include "GLError.hpp"

#include <Rk/types.hpp>

#include <memory>

#include <GL/glew.h>

namespace Ad
{
  class Buffer
  {
    uint glname;

  public:
    typedef std::shared_ptr <Buffer> Ptr;

    Buffer (size_t size, const void* data = nullptr)
    {
      glGenBuffers (1, &glname);
      check_gl ("glGenBuffers failed");

      glBindBuffer (GL_ARRAY_BUFFER, glname);
      check_gl ("glBindBuffer failed");

      auto mode = data ? GL_STATIC_DRAW : GL_STREAM_DRAW;

      glBufferData (GL_ARRAY_BUFFER, size, data, mode);
      check_gl ("glBufferData failed");
    }

    ~Buffer ()
    {
      if (glname != 0)
        glDeleteBuffers (1, &glname);
    }

    uint name () const
    {
      return glname;
    }

    uint release ()
    {
      auto temp = glname;
      glname = 0;
      return temp;
    }

    static Ptr create (size_t size, const void* data = nullptr)
    {
      return std::make_shared <Buffer> (size, data);
    }

  };

}
