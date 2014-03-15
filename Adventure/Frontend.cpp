//
// Adventure
//

#include <Rk/memory.hpp>
#include <Rk/clamp.hpp>
#include <Rk/types.hpp>

#include "Render.hpp"
#include "Window.hpp"
#include "Phase.hpp"

namespace Ad
{
  extern "C"
  {
    __declspec(dllimport) i32 __stdcall QueryPerformanceFrequency (u64*);
    __declspec(dllimport) i32 __stdcall QueryPerformanceCounter   (u64*);
  }

  class Clock
  {
    u64 prev_ticks,
        ticks_per_second;

  public:
    Clock ()
    {
      QueryPerformanceFrequency (&ticks_per_second);
    }

    u64 ticks () const
    {
      u64 cur_ticks;
      QueryPerformanceCounter (&cur_ticks);
      return cur_ticks;
    }

    u64 frequency () const
    {
      return ticks_per_second;
    }

  };

  class Syncer
  {
    Clock  clock;
    u64    prev_ticks;
    float  sim_time,
           delta,
           accum,
           catchup,
           wrap;
    bool   ticking;

  public:
    Syncer (float tick_frequency, float max_catchup = 0.25f, float wrap_interval = 12000.0f) :
      delta   (1 / tick_frequency),
      ticking (false),
      catchup (max_catchup),
      wrap    (wrap_interval)
    {
      restart ();
    }

    void restart ()
    {
      sim_time = 0;
      accum    = 0;

      prev_ticks = clock.ticks ();
    }

    float interval () const
    {
      return delta;
    }

    float time () const
    {
      return sim_time;
    }

    float alpha () const
    {
      return accum / delta;
    }

    void update_clock ()
    {
      auto ticks = clock.ticks ();
      auto delta_ticks = ticks - prev_ticks;
      prev_ticks = ticks;

      auto delta_real_time = float (delta_ticks) / float (clock.frequency ());

      delta_real_time = Rk::clamp (delta_real_time, 0.0f, catchup);
      accum += delta_real_time;

      ticking = false;
    }

    bool need_tick () const
    {
      return accum >= delta;
    }

    void tick_done ()
    {
      accum    -= delta;
      sim_time += delta;

      // Wrap predictably
      if (sim_time >= wrap)
        sim_time -= wrap;
    }

  };

  class Frontend
  {
    Window     win;
    bool       running;
    Phase::Ptr phase;
    Syncer     syncer;

  public:
    Frontend () :
      win    ("Adventure"),
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
        win.pump ();

        // Update input

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
        win.flip ();
      }
    }

  };

  void true_main ()
  {
    Frontend fe;
    fe.run ();
  }

}
