//
// Adventure
//

#include "Texture.hpp"

#include "stb_image.hpp"
#include "GLError.hpp"

#include <Rk/file_stream.hpp>
#include <Rk/exception.hpp>
#include <Rk/guard.hpp>

#include <GL/glew.h>

#include <vector>

namespace Ad
{
  Texture::Texture (Rk::cstring_ref path, int flags)
  {
    auto target = GL_TEXTURE_2D;

    Rk::fio::in_stream file (path);
    std::vector <u8> raw ((u32) file.size ());
    file.read (raw.data (), raw.size ());
    
    int comp, glformat;

    if (flags & TexFlag::alpha)
    {
      comp = 1;
      glformat = GL_ALPHA;
    }
    else
    {
      comp = 4;
      glformat = GL_RGBA;
    }

    auto pixels = stbi_load_from_memory (raw.data (), raw.size (), &wide, &high, &comp, comp);

    if (!pixels)
      throw std::runtime_error ("Error loading texture image");
    auto pixel_guard = Rk::guard ([=] { free (pixels); });

    glGenTextures (1, &glname);
    check_gl ("glGenTextures failed");

    glBindTexture (target, glname);
    check_gl ("glBindTexture failed");

    int glwrap;
    if (flags & TexFlag::clamp)
      glwrap = GL_CLAMP_TO_EDGE;
    else
      glwrap = GL_REPEAT;

    int glfilt;
    if (flags & TexFlag::nearest)
      glfilt = GL_NEAREST;
    else
      glfilt = GL_LINEAR;

    glTexParameteri (target, GL_TEXTURE_MAX_LEVEL,  0);
    glTexParameteri (target, GL_TEXTURE_MIN_FILTER, glfilt);
    glTexParameteri (target, GL_TEXTURE_MAG_FILTER, glfilt);
    glTexParameteri (target, GL_TEXTURE_WRAP_S,     glwrap);
    glTexParameteri (target, GL_TEXTURE_WRAP_T,     glwrap);
    check_gl ("glTexParameteri failed");

    glTexImage2D (target, 0, glformat, wide, high, 0, glformat, GL_UNSIGNED_BYTE, pixels);
    check_gl ("glTexImage2D failed");

    glBindTexture (target, 0);
  }

  Texture::~Texture ()
  {
    glDeleteTextures (1, &glname);
  }

}
