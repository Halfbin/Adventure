//
// Adventure
//

#include "Phase.hpp"

#include "Texture.hpp"

#include <Rk/lerp.hpp>

#include <GL/glew.h>

namespace Ad
{
  class Menu :
    public Phase
  {
    Texture::Ptr logo, floor;

    float t0, t1;

    void input (const Event*, uint, const KeyState*)
    {

    }

    void tick (float time, float step)
    {
      t0 = time;
      t1 = time + step;
    }

    void render (Frame& frame)
    {
      /*glClearColor (0.00f, 0.26f, 0.51f, 1.0f);
      glClear (GL_COLOR_BUFFER_BIT);

      glMatrixMode (GL_PROJECTION);
      glLoadIdentity ();
      glOrtho (-(width/2), width/2, height/2, -(height/2), -1, 1);

      glMatrixMode (GL_MODELVIEW);
      glLoadIdentity ();
      auto theta = Rk::lerp (t0, t1, alpha);
      glRotatef (theta * 100, 0.0f, 0.0f, 1.0f);*/

      /*glBindTexture (GL_TEXTURE_2D, floor -> name ());

      glBegin (GL_QUADS);
        glTexCoord2i (0, 0); glVertex2i (  0,   0);
        glTexCoord2i (0, 30); glVertex2i (  0, 960);
        glTexCoord2i (30, 30); glVertex2i (960, 960);
        glTexCoord2i (30, 0); glVertex2i (960,   0);
      glEnd ();*/

      /*glBindTexture (GL_TEXTURE_2D, logo -> name ());
      glEnable (GL_TEXTURE_2D);

      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable (GL_BLEND);

      glBegin (GL_QUADS);
        glTexCoord2i (0, 0); glVertex2i (-170, -170);
        glTexCoord2i (0, 1); glVertex2i (-170,  170);
        glTexCoord2i (1, 1); glVertex2i ( 170,  170);
        glTexCoord2i (1, 0); glVertex2i ( 170, -170);
      glEnd ();

      glDisable (GL_BLEND);*/


    }

  public:
    Menu ()
    {
      using namespace TexFlags;
      logo = Texture::create ("Art/Logo2.png", clamp);
      floor = Texture::create ("Art/Floor.png", repeat);
    }

    ~Menu () = default;

  };

  Phase::Ptr create_menu ()
  {
    return std::make_unique <Menu> ();
  }

}
