//
// Adventure
//

#include <Rk/clamp.hpp>
#include <Rk/types.hpp>

#include <GL/glew.h>

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
    u64   start;
    float ticks_per_second;

  public:
    Clock ()
    {
      u64 perf_freq;
      QueryPerformanceFrequency (&perf_freq);
      ticks_per_second = float (perf_freq);
      restart ();
    }

    void restart ()
    {
      QueryPerformanceCounter (&start);
    }

    __forceinline float time () const
    {
      u64 count;
      QueryPerformanceCounter (&count);
      return float (count - start) / ticks_per_second;
    }

  };

  class Syncer
  {
    Clock clock;
    float prev_real_time,
          sim_time,
          delta,
          accum;
    bool  ticking;

  public:
    Syncer (float tick_frequency) :
      delta   (1 / tick_frequency),
      ticking (false)
    {
      restart ();
    }

    void restart ()
    {
      sim_time = 0;
      accum    = 0;

      prev_real_time = clock.time ();
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
      float real_time = clock.time ();
      float delta_real_time = real_time - prev_real_time;
      prev_real_time = real_time;

      delta_real_time = Rk::clamp (delta_real_time, 0.0f, 0.25f);
      accum += delta_real_time;

      ticking = false;
    }

    bool tick ()
    {
      if (ticking)
      {
        accum    -= delta;
        sim_time += delta;
      }

      ticking = accum >= delta;

      return ticking;
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

      while (running)
      {
        win.pump ();

        // Update input

        // Update clock
        syncer.update_clock ();

        // Do tick(s)
        while (syncer.tick ())
          phase -> tick (syncer.time (), syncer.interval ());

        // Render AV
        glViewport (0, 0, win.width (), win.height ());
        phase -> render (win.width (), win.height ());
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
