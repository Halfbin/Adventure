//
// Adventure
//

#include "Render.hpp"

#include <Rk/transform.hpp>

#include "ModelShader.hpp"
#include "StarfieldShader.hpp"

#include "GLError.hpp"
#include "Buffer.hpp"

#include <GL/glew.h>

namespace Ad
{
  const auto vbo_size = 4096;

  struct Renderer::Impl
  {
  //Buffer          vbo;
    Frame           frame;
  //DefaultShader   shader;
    ModelShader     model_shader;
    StarfieldShader starfield_shader;

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

    void render_models    (const Frame& frame);
    void render_starfield (const Frame& frame);

  };

  void Renderer::Impl::render_models (const Frame& frame)
  {
    model_shader.use ();

    glActiveTexture (GL_TEXTURE0 + model_shader.tex_unit);

    glEnable (GL_CULL_FACE);
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    uint prev_layer = 0xffffffff;
    m4f world_to_clip;

    for (auto item : frame.model_items)
    {
      if (item.layer != prev_layer)
      {
        glClear (GL_DEPTH_BUFFER_BIT);
        auto eye_to_clip = Rk::eye_to_clip (75.0f, float (frame.width) / float (frame.height), frame.camera_near [item.layer], frame.camera_far [item.layer]);
        auto world_to_eye = Rk::world_to_eye (frame.camera_pos [item.layer], frame.camera_ori);
        world_to_clip = eye_to_clip * world_to_eye;

        prev_layer = item.layer;
      }

      auto model_to_world = Rk::affine (item.trn, item.rot);

      glUniformMatrix4fv (model_shader.model_to_world (), 1, true, reinterpret_cast <const float*> (&model_to_world));
      check_gl ("glUniformMatrix4fv failed");

      for (int i = 0; i != 3; i++)
      for (int j = 0; j != 3; j++)
        model_to_world (i, j) *= item.scale [i];

      auto model_to_clip = world_to_clip * model_to_world;

      glUniformMatrix4fv (model_shader.model_to_clip (), 1, true, reinterpret_cast <const float*> (&model_to_clip));
      check_gl ("glUniformMatrix4fv failed");

      v4f col { 1.0f, 0.0f, 0.0f, 1.0f };
      glVertexAttrib4fv (model_shader.attrib_colour, reinterpret_cast <const float*> (&col));
      check_gl ("glVertexAttrib4f failed");

      glBindTexture (GL_TEXTURE_2D, 0);//item.tex);
      check_gl ("glBindTexture failed");

      glVertexAttrib4f (model_shader.attrib_normal, 0, 0, 0, 0);

      glBindVertexArray (item.geom);
      check_gl ("glBindVertexArray failed");

      for (auto i_mesh = item.first_mesh; i_mesh != item.first_mesh + item.mesh_count; i_mesh++)
      {
        const auto& mesh = frame.mesh_items [i_mesh];

        glDrawElementsBaseVertex (mesh.type, mesh.count, item.idx_type, (const void*) uptr (mesh.first * 2), mesh.base);
        check_gl ("glDrawElements failed");
      }
    }
  }

  void Renderer::Impl::render_starfield (const Frame& frame)
  {
    m4f e2c = Rk::eye_to_clip (75.0f, float (frame.width) / float (frame.height), 0.1f, 1000.0f);
    m4f s2e = Rk::world_to_eye (v3f {0,0,0}, frame.camera_ori);
    auto s2c = e2c * s2e;

    starfield_shader.use ();

    glUniform1f (starfield_shader.twinkle (), 1.0f);

    glUniformMatrix4fv (starfield_shader.sky_to_clip (), 1, true, reinterpret_cast <const float*> (&s2c));

    glEnable (GL_PROGRAM_POINT_SIZE);
    glDisable (GL_CULL_FACE);
    glEnable  (GL_DEPTH_TEST);
    glDisable (GL_BLEND);

    glBindVertexArray (frame.starfield_geom);
    check_gl ("glBindVertexArray failed");

    glDrawArrays (GL_POINTS, 0, frame.starfield_size);
    check_gl ("glDrawElements failed");
  }

  Frame& Renderer::begin (int width, int height, float alpha)
  {
    auto& frame = impl->frame;

    frame.width  = width;
    frame.height = height;
    frame.alpha  = alpha;

    frame.model_items.clear ();

    frame.starfield_geom = 0;
    frame.starfield_size = 0;

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
    auto& frame = impl->frame;
    
  //auto ok = glUnmapBuffer (GL_ARRAY_BUFFER);

    glViewport (0, 0, frame.width, frame.height);

    // Sort models by layer, back-to-front
    auto& models = impl->frame.model_items;
    std::sort (
      models.begin (),
      models.end (),
      [] (const Frame::ModelItem& lhs, const Frame::ModelItem& rhs) { return lhs.layer < rhs.layer; }
    );

    auto cc = frame.clear_colour;
    glClearColor (cc.x, cc.y, cc.z, cc.w);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    impl -> render_starfield (frame);

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
