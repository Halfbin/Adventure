//
// Adventure
//

#include "Frontend.hpp"

#include <Rk/memory.hpp>
#include <Rk/clamp.hpp>
#include <Rk/types.hpp>

#include "InputSystem.hpp"
#include "GLContext.hpp"
#include "Render.hpp"
#include "Window.hpp"
#include "Clock.hpp"
#include "Phase.hpp"

namespace Ad
{
  class FrontendImpl :
    public Frontend
  {
    InputSystem in_sys;
    Window      win;
    GLContext   gl_ctx;
    bool        running;
    Phase::Ptr  phase;
    Syncer      syncer;

  public:
    FrontendImpl () :
      win    ("Adventure", in_sys),
      gl_ctx (win.handle ()),
      syncer (50)
    {
      win.on_close ([this] { running = false; });

      phase = create_phase_0 ();
    }

    void run ()
    {
      running = true;
      win.show ();
      syncer.restart ();
      Renderer renderer;

      while (running)
      {
        // Update input
        auto in_ctx = win.pump (*this);
        phase -> input (in_ctx);

        // Update clock
        syncer.update_clock ();

        // Do tick(s)
        while (syncer.need_tick ())
        {
          phase -> tick (syncer.time (), syncer.interval ());
          syncer.tick_done ();
        }

        // Render AV
        auto& frame = renderer.begin (win.width (), win.height (), syncer.alpha ());
        phase -> render (frame);
        renderer.end ();
        gl_ctx.flip ();
      }
    }

    void enter_mouse_look ()
    {
      in_sys.enter_mouse_look (win.handle ());
    }

    void leave_mouse_look ()
    {
      in_sys.leave_mouse_look (win.handle ());
    }

  };

  void true_main ()
  {
    FrontendImpl fe;
    fe.run ();
  }

}
