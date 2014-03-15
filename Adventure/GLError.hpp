//
// Adventure
//

#pragma once

#include <Rk/tio/format.hpp>

#include <stdexcept>

#include <GL/glew.h>

namespace Ad
{
  class GLError :
    public std::runtime_error
  {
  public:
    GLError (Rk::cstring_ref message, GLenum code = glGetError ()) :
      std::runtime_error (Rk::tio::format ("%1: OpenGL error code %2", message, int (code)))
    { }

  };

  inline void check_gl (Rk::cstring_ref message, GLenum code = glGetError ())
  {
    if (code != GL_NO_ERROR)
      throw GLError (message, code);
  }

}
