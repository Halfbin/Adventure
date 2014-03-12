//
// Adventure
//

#include "Phase.hpp"

#include "Texture.hpp"

#include <GL/glew.h>

namespace Ad
{
  class Menu :
    public Phase
  {
    Texture::Ptr logo;

    void tick (float time, float step)
    {

    }

    void render (int width, int height)
    {
      glClearColor (0.00f, 0.26f, 0.51f, 1.0f);
      glClear (GL_COLOR_BUFFER_BIT);

      glMatrixMode (GL_PROJECTION);
      glLoadIdentity ();
      glOrtho (0, width, height, 0, -1, 1);

      glMatrixMode (GL_MODELVIEW);
      glLoadIdentity ();

      glBindTexture (GL_TEXTURE_2D, logo -> name ());
      glEnable (GL_TEXTURE_2D);

      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable (GL_BLEND);

      glBegin (GL_QUADS);
        glTexCoord2i (0, 0); glVertex2i (  0,   0);
        glTexCoord2i (0, 1); glVertex2i (  0, 340);
        glTexCoord2i (1, 1); glVertex2i (340, 340);
        glTexCoord2i (1, 0); glVertex2i (340,   0);
      glEnd ();

      glDisable (GL_BLEND);
    }

  public:
    Menu ()
    {
      logo = Texture::create ("Art/Logo.png");
    }

    ~Menu () = default;

  };

  Phase::Ptr create_menu ()
  {
    return std::make_unique <Menu> ();
  }

}
