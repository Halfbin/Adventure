//
// Adventure
//

#include "Render.hpp"

#include <Rk/transform.hpp>

#include "DefaultShader.hpp"
#include "ModelShader.hpp"

#include "GLError.hpp"
#include "Buffer.hpp"

#include <GL/glew.h>

namespace Ad
{
  const auto vbo_size = 4096;

  struct Renderer::Impl
  {
    Buffer        vbo;
    Frame         frame;
    DefaultShader shader;
    ModelShader   model_shader;

    Impl () :
      vbo (vbo_size)
    {
      glBindVertexArray (0);
      glBindBuffer (GL_ARRAY_BUFFER, vbo.name ());

      glVertexAttribPointer (shader.attrib_vertpos, 2, GL_FLOAT, false, 16, (void*) 0);
      check_gl ("glVertexAttribIPointer failed");

      glEnableVertexAttribArray (shader.attrib_vertpos);
      check_gl ("glEnableVertexAttribArray failed");

      glVertexAttribPointer (shader.attrib_tcoords, 2, GL_FLOAT, false, 16, (void*) 8);
      check_gl ("glVertexAttribIPointer failed");

      glEnableVertexAttribArray (shader.attrib_tcoords);
      check_gl ("glEnableVertexAttribArray failed");
    }

    void render_2d     (const Frame& frame);
    void render_models (const Frame& frame);

  };

  void Renderer::Impl::render_2d (const Frame& frame)
  {
    auto c = frame.camera;

    m4f w2e = Rk::matrix_rows {
      v4f { 1, 0, 0, -c.x },
      v4f { 0, 1, 0, -c.y },
      v4f { 0, 0, 1,   0  },
      v4f { 0, 0, 0,   1  }
    };

    auto w2c = frame.eye_to_clip * w2e;

    shader.use ();

    glActiveTexture (GL_TEXTURE0 + shader.tex_unit);

    glDisable (GL_CULL_FACE);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    
    int first = 0;

    for (auto item : frame.draw_items)
    {
      auto rot = item.rotate;
      auto trn = item.translate;

      auto c = item.rotate.real ();
      auto s = item.rotate.imag ();

      m4f m2w = Rk::matrix_rows {
        v4f { c, -s, 0, trn.x },
        v4f { s,  c, 0, trn.y },
        v4f { 0,  0, 1,   0   },
        v4f { 0,  0, 0,   1   }
      };

      auto m2c = w2c * m2w;

      glUniformMatrix4fv (shader.model_to_clip (), 1, true, reinterpret_cast <const float*> (&m2c));
      check_gl ("glUniformMatrix4fv failed");

      glBindTexture (GL_TEXTURE_2D, item.texture);
      check_gl ("glBindTexture failed");

      glDrawArrays (GL_TRIANGLE_FAN, first, item.count);
      check_gl ("glDrawArrays failed");

      first += item.count;
    }
  }

  void Renderer::Impl::render_models (const Frame& frame)
  {
    auto h = 0.707f;
    auto c = frame.camera;

    m4f w2e = Rk::matrix_rows {
      v4f { 1,  0,  0, -c.x },
      v4f { 0,  h,  h, -c.y },
      v4f { 0, -h, -h,   0  },
      v4f { 0,  0,  0,   1  }
    };

    auto w2c = frame.eye_to_clip * w2e;

    model_shader.use ();

    glEnable (GL_CULL_FACE);
    
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

      glBindVertexArray (item.geom);
      check_gl ("glBindVertexArray failed");

      glDrawElements (GL_TRIANGLES, item.n_idxs, item.idx_type, nullptr);
      check_gl ("glDrawElements failed");
    }
  }

  Frame& Renderer::begin (int width, int height, float alpha)
  {
    auto& frame = impl -> frame;

    frame.width  = width;
    frame.height = height;
    frame.alpha  = alpha;

    frame.draw_items.clear ();
    frame.model_items.clear ();

    glBindVertexArray (0);

    glBindBuffer (GL_ARRAY_BUFFER, impl -> vbo.name ());
    check_gl ("glBindBuffer failed");

    frame.buf_ptr = (u8*) glMapBufferRange (GL_ARRAY_BUFFER, 0, vbo_size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    if (!frame.buf_ptr)
      throw GLError ("glMapBufferRange failed");

    frame.buf_end = frame.buf_ptr + vbo_size;

    return frame;
  }

  void Renderer::end ()
  {
    auto& frame = impl -> frame;
    auto& shader = impl -> shader;
    
    auto ok = glUnmapBuffer (GL_ARRAY_BUFFER);

    glViewport (0, 0, frame.width, frame.height);

    auto cc = frame.clear_colour;
    if (!ok) cc = cc (2,1,0,3);
    glClearColor (cc.x, cc.y, cc.z, cc.w);
    glClear (GL_COLOR_BUFFER_BIT);

    impl -> render_2d     (frame);
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
