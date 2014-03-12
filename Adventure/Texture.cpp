//
// Adventure
//

#include "Texture.hpp"

#include "stb_image.hpp"

#include <Rk/file_stream.hpp>
#include <Rk/exception.hpp>
#include <Rk/guard.hpp>

#include <GL/glew.h>

#include <vector>

namespace Ad
{
  Texture::Texture (Rk::cstring_ref path)
  {
    Rk::fio::in_stream file (path);
    std::vector <u8> raw ((u32) file.size ());
    file.read (raw.data (), raw.size ());
    
    int width, height, comp;
    auto pixels = stbi_load_from_memory (raw.data (), raw.size (), &width, &height, &comp, 4);

    if (!pixels)
      throw std::runtime_error ("Error loading texture image");
    auto pixel_guard = Rk::guard ([=] { free (pixels); });

    glGenTextures (1, &glname);
    glBindTexture (GL_TEXTURE_2D, glname);

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,  0);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glBindTexture (GL_TEXTURE_2D, 0);
  }

  Texture::~Texture ()
  {
    glDeleteTextures (1, &glname);
  }

}
