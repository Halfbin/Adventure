//
// Adventure
//

#include "Render.hpp"

#include <Rk/transform.hpp>

#include "ModelShader.hpp"

#include "GLError.hpp"
#include "Buffer.hpp"

#include <GL/glew.h>

namespace Ad
{
  const auto vbo_size = 4096;

  struct Renderer::Impl
  {
  //Buffer        vbo;
    Frame         frame;
  //DefaultShader shader;
    ModelShader   model_shader;

    Impl ()/* :
      vbo (vbo_size)*/
    {
    /*glBindVertexArray (0);
      glBindBuffer (GL_ARRAY_BUFFER, vbo.name ());

      glVertexAttribPointer (shader.attrib_vertpos, 2, GL_FLOAT, false, 16, (void*) 0);
      check_gl ("glVertexAttribIPointer failed");

      glEnableVertexAttribArray (shader.attrib_vertpos);
      check_gl ("glEnableVertexAttribArray failed");

      glVertexAttribPointer (shader.attrib_tcoords, 2, GL_FLOAT, false, 16, (void*) 8);
      check_gl ("glVertexAttribIPointer failed");

      glEnableVertexAttribArray (shader.attrib_tcoords);
      check_gl ("glEnableVertexAttribArray failed");*/
    }

    void render_models (const Frame& frame);

  };

  void Renderer::Impl::render_models (const Frame& frame)
  {
    m4f e2c = Rk::eye_to_clip (75.0f, float (frame.width) / float (frame.height), 0.1f, 1000.0f);
    m4f w2e = Rk::world_to_eye (frame.camera_pos, frame.camera_ori);
    auto w2c = e2c * w2e;

    model_shader.use ();

    glActiveTexture (GL_TEXTURE0 + model_shader.tex_unit);

    glEnable (GL_CULL_FACE);
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (auto item : frame.model_items)
    {
      auto m2w = Rk::affine (item.trn, item.rot);

      glUniformMatrix4fv (model_shader.model_to_world (), 1, true, reinterpret_cast <const float*> (&m2w));
      check_gl ("glUniformMatrix4fv failed");

      for (int i = 0; i != 3; i++)
      for (int j = 0; j != 3; j++)
        m2w (i, j) *= item.scale [i];

      auto m2c = w2c * m2w;

      glUniformMatrix4fv (model_shader.model_to_clip (), 1, true, reinterpret_cast <const float*> (&m2c));
      check_gl ("glUniformMatrix4fv failed");

      glVertexAttrib4fv (model_shader.attrib_colour, reinterpret_cast <const float*> (&item.colour));
      check_gl ("glVertexAttrib4f failed");

      glBindTexture (GL_TEXTURE_2D, item.tex);
      check_gl ("glBindTexture failed");

      glVertexAttrib4f (model_shader.attrib_normal, 0, 0, 0, 0);

      glBindVertexArray (item.geom);
      check_gl ("glBindVertexArray failed");

      glDrawElementsBaseVertex (GL_TRIANGLES, item.idx_count, item.idx_type, nullptr, item.first_idx);
      check_gl ("glDrawElements failed");
    }
  }

  Frame& Renderer::begin (int width, int height, float alpha)
  {
    auto& frame = impl -> frame;

    frame.width  = width;
    frame.height = height;
    frame.alpha  = alpha;

    frame.model_items.clear ();

  /*glBindVertexArray (0);

    glBindBuffer (GL_ARRAY_BUFFER, impl -> vbo.name ());
    check_gl ("glBindBuffer failed");

    frame.buf_ptr = (u8*) glMapBufferRange (GL_ARRAY_BUFFER, 0, vbo_size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    if (!frame.buf_ptr)
      throw GLError ("glMapBufferRange failed");

    frame.buf_end = frame.buf_ptr + vbo_size;*/

    return frame;
  }

  void Renderer::end ()
  {
    auto& frame = impl -> frame;
    
  //auto ok = glUnmapBuffer (GL_ARRAY_BUFFER);

    glViewport (0, 0, frame.width, frame.height);

    auto cc = frame.clear_colour;
    glClearColor (cc.x, cc.y, cc.z, cc.w);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    impl -> render_models (frame);
  }

  Renderer::Renderer () :
    impl (new Impl)
  { }

  Renderer::~Renderer ()
  {
    delete impl;
  }

}
