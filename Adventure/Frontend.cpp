//
// Adventure
//

#include <Rk/types.hpp>

#include <GL/glew.h>

#include "Window.hpp"

namespace Ad
{
  class Frontend
  {
    Window win;
    bool   running;

  public:
    Frontend () :
      win ("Adventure")
    {
      win.on_close ([this] { running = false; });
    }

    void run ()
    {
      running = true;
      win.show ();

      while (running)
      {
        win.pump ();

        // Update input

        // Update clock

        // Do tick(s)

        // Render AV
        glViewport (0, 0, win.width (), win.height ());
        glClearColor (0, 0, 0, 0);
        glClear (GL_COLOR_BUFFER_BIT);



        win.flip ();
      }
    }

  };

  void true_main ()
  {
    glewInit ();

    Frontend fe;
    fe.run ();
  }

}
