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
#include "INI.hpp"

namespace Ad
{
  class FrontendImpl :
    public Frontend
  {
    std::string data_path;

    InputSystem in_sys;
    Window      win;
    GLContext   gl_ctx;
    bool        running;
    Phase::Ptr  phase;
    Syncer      syncer;

    void load_paths (INILoader& ini)
    {
      for (;;)
      {
        auto stat = ini.proceed ();
        if (stat == INIStatus::done_section)
        {
          break;
        }
        else if (stat == INIStatus::got_pair)
        {
          if (ini.key () == "data")
            data_path = to_string (ini.value ());
        }
      }

      if (data_path.empty ())
        data_path = ".";

      if (data_path.back () == '\\')
        data_path.pop_back ();

      if (data_path.back () != '/')
        data_path.push_back ('/');
    }

    void load_config (Rk::cstring_ref path)
    {
      INILoader ini (path);

      for (;;)
      {
        auto stat = ini.proceed ();
        if (stat == INIStatus::done)
        {
          break;
        }
        else if (stat == INIStatus::got_section)
        {
          if (ini.section () == "Paths")
            load_paths (ini);
        }
      }
    }

  public:
    FrontendImpl () :
      win    ("Adventure", in_sys),
      gl_ctx (win.handle ()),
      syncer (50)
    {
      win.on_close ([this] { running = false; });

      load_config ("Adventure.ini");

      InitContext ctx = { data_path };
      phase = create_phase_0 (ctx);
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
