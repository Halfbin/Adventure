//
// Adventure
//

#include "Shader.hpp"

// Uses
#include "GLError.hpp"

#include <Rk/file_stream.hpp>

#include <vector>

#include <GL/glew.h>

namespace Ad
{
  Shader::Shader (Rk::cstring_ref path, uint type)
  {
    Rk::fio::in_stream file (path);

    u32 size = u32 (file.size ());
    if (size == 0)
      throw std::runtime_error ("Shader source is empty");

    std::vector <char> buffer (size);
    file.read (buffer.data (), size);

    glname = glCreateShader (type);
    if (!glname)
      throw std::runtime_error ("glCreateShader failed");

    const char* data = buffer.data ();
    u32 len = size;
    glShaderSource (glname, 1, &data, (const i32*) &len);
    check_gl ("glShaderSource failed");

    glCompileShader (glname);
    check_gl ("glCompileShader failed");

    i32 loglen;
    glGetShaderiv (glname, GL_INFO_LOG_LENGTH, &loglen);

    if (loglen > 1)
    {
      buffer.resize (loglen);
      data = buffer.data ();
      glGetShaderInfoLog (glname, loglen, &loglen, buffer.data ());
    }

    int ok;
    glGetShaderiv (glname, GL_COMPILE_STATUS, &ok);

    if (!ok)
      throw std::runtime_error ("Error compiling shader");
  }

  Shader::~Shader ()
  {
    glDeleteShader (glname);
  }

  ShaderProgram::ShaderProgram ()
  {
    glname = glCreateProgram ();
    check_gl ("glCreateProgram failed");
  }

  ShaderProgram::~ShaderProgram ()
  {
    glDeleteProgram (glname);
  }

  void ShaderProgram::add_shader (const Shader& shader)
  {
    glAttachShader (glname, shader.name ());
    check_gl ("glAttachShader failed");
  }

  void ShaderProgram::fix_attrib (const char* attrib, u32 location)
  {
    glBindAttribLocation (glname, location, attrib);
    check_gl ("glBindAttribLocation failed");
  }

  void ShaderProgram::link ()
  {
    glLinkProgram (glname);

    u32 len;
    glGetProgramiv (glname, GL_INFO_LOG_LENGTH, (i32*) &len);

    if (len > 1)
    {
      std::vector <char> buffer (len);
      glGetProgramInfoLog (glname, len, (i32*) &len, buffer.data ());
    }

    int ok;
    glGetProgramiv (glname, GL_LINK_STATUS, &ok);

    if (!ok)
      throw std::runtime_error ("Error linking shader program");
  }

  u32 ShaderProgram::link_uniform (const char* uniform)
  {
    u32 location = glGetUniformLocation (glname, uniform);
    if (location == 0xffffffff)
      throw std::runtime_error ("Uniform not present or inactive");
    return location;
  }

  void ShaderProgram::use ()
  {
    glUseProgram (glname);
  }

  void ShaderProgram::done ()
  {
    glUseProgram (0);
  }

}
