//
// Adventure
//

#include "Phase.hpp"

#include "ModelShader.hpp"
#include "Starfield.hpp"
#include "Universe.hpp"
#include "Texture.hpp"
#include "Buffer.hpp"
#include "Player.hpp"
#include "Geom.hpp"
#include "Item.hpp"
#include "INI.hpp"

#include <iomanip>
#include <sstream>

#include <Rk/transform.hpp>

#include <GL/glew.h>

namespace Ad
{
  namespace
  {
    // shit.
    void grab_int (int& value, Rk::cstring_ref src, int min = INT_MIN, int max = INT_MAX)
    {
      auto str = Rk::to_string (src);
      std::istringstream ss (str);
      ss >> value;
      if (ss.bad () || !ss.eof ())
        throw std::runtime_error ("Error parsing int in INI");
      if (value < min || value > max)
        throw std::runtime_error ("Integer out-of-range in INI");
    }

    void grab_float (float& value, Rk::cstring_ref src, float min = -FLT_MAX, float max = FLT_MAX)
    {
      auto str = Rk::to_string (src);
      std::istringstream ss (str);
      ss >> value;
      if (ss.bad () || !ss.eof ())
        throw std::runtime_error ("Error parsing float in INI");
      if (value < min || value > max)
        throw std::runtime_error ("Float out-of-range in INI");
    }

    void grab_v3i (v3i& value, Rk::cstring_ref src, v3i mins = v3i {INT_MIN,INT_MIN,INT_MIN}, v3i maxs = v3i {INT_MAX,INT_MAX,INT_MAX})
    {
      auto str = Rk::to_string (src);
      std::istringstream ss (str);
      ss.setf (ss.skipws);
      ss >> value.x; ss.ignore (5000, ',');
      ss >> value.y; ss.ignore (5000, ',');
      ss >> value.z;
      if (ss.bad () || !ss.eof ())
        throw std::runtime_error ("Error parsing v3i in INI");
      if (Rk::inner (std::logical_or <> (), std::less <> (), value, mins) || Rk::inner (std::logical_or <> (), std::greater <> (), value, maxs))
        throw std::runtime_error ("v3i out-of-range in INI");
    }

  }

  extern "C" __declspec (dllimport) void __stdcall OutputDebugStringA (const char*);

  class CollisionState
  {
  public:
    struct Event { Entity* ent; v3f normal; float pdist; };

  private:
    struct Plane { v3f normal; float dist; };
    enum { max_planes = 100 };
    Plane planes [max_planes];
    uint n_planes;

    struct BBox { Entity* ent; v3f mins, maxs; };
    enum { max_ents = 200 };
    BBox boxes [max_ents];
    uint n_boxes;

    enum { max_events = max_ents };
    Event events [max_events];
    uint n_events;

  public:
    CollisionState ()
    {
      reset ();
      n_planes = 0;
    }

    void reset ()
    {
      n_boxes = 0;
      n_events = 0;
    }

    void add_plane (v3f normal, float dist)
    {
      planes [n_planes++] = { normal, dist };
    }

    void add_ent (Entity* ent)
    {
      boxes [n_boxes++] = { ent, ent->bound_mins (), ent->bound_maxs () };
    }
    
    struct Collisions
    {
      const Event* bg;
      const Event* ed;

      const Event* begin () const { return bg; }
      const Event* end   () const { return ed; }

    };

    Collisions find_collisions ()
    {
      for (int i_plane = 0; i_plane != n_planes; i_plane++)
      {
        const auto plane = planes [i_plane];

        for (int i_box = 0; i_box != n_boxes; i_box++)
        {
          const auto box = boxes [i_box];

          v3f deepest = {
            plane.normal.x > 0 ? box.mins.x : box.maxs.x,
            plane.normal.y > 0 ? box.mins.y : box.maxs.y,
            plane.normal.z > 0 ? box.mins.z : box.maxs.z
          };

          auto dist = dot (deepest, plane.normal) - plane.dist;

          if (dist < 0)
            events [n_events++] = { box.ent, plane.normal, -dist };
        }
      }

      return Collisions { events, events + n_events };
    }

  };

  class PlayPhase :
    public Phase
  {
    UniverseCfg universe_cfg;

    Player::Ptr player;
    std::vector <Entity::Ptr> entities;
    CollisionState collide;

    vec4f background_colour;
    Geom starfield;

    float t0, t1;

    void add_entity (Entity::Ptr ent)
    {
      entities.push_back (std::move (ent));
    }

    void input (const InputContext& ctx)
    {
      for (const auto& ev : ctx.events)
      {
        if (is_button_down (ev, Buttons::left))
          ctx.frontend.enter_mouse_look ();
        else if (is_key_up (ev, Keys::escape))
          ctx.frontend.leave_mouse_look ();
      }

      player -> input (ctx);
    }

    void tick (float time, float step)
    {
      t0 = time;
      t1 = time + step;

      collide.reset ();

      for (auto&& ent : entities)
      {
        ent -> advance (time, step);
        collide.add_ent (ent.get ());
      }

      player -> advance (time, step);
      collide.add_ent (player.get ());

      for (auto ev : collide.find_collisions ())
        ev.ent -> collide (ev.normal, ev.pdist);
    }

    void render (Frame& frame)
    {
      frame.clear_colour = background_colour;
      frame.set_starfield (starfield);

      for (auto&& ent : entities)
        ent -> draw (frame);

      player -> draw (frame);
    }

    void configure_starfield (INILoader& ini)
    {
      int   size = 0,
            seed = 0;
      float lambda = 3.5f,
            scale = 1.2f;

      INIStatus stat;
      while ((stat = ini.proceed ()) == INIStatus::got_pair)
      {
        if (ini.key () == "size")
          grab_int (size, ini.value (), 1);
        else if (ini.key () == "seed")
          grab_int (seed, ini.value ());
        else if (ini.key () == "lambda")
          grab_float (lambda, ini.value (), 0.0f, 10.0f);
        else if (ini.key () == "scale")
          grab_float (scale, ini.value (), 0.0f, 100.0f);
      }

      starfield = make_starfield (size, seed, lambda, scale);
    }

    void configure_background (INILoader& ini)
    {
      vec3i bgcol;

      INIStatus stat;
      while ((stat = ini.proceed ()) == INIStatus::got_pair)
      {
        if (ini.key () == "colour")
          grab_v3i (bgcol, ini.value (), nil, v3i {255,255,255});
      }

      background_colour = compose_vector (bgcol / 255.0f, 1.0f);
    }

  public:
    PlayPhase (InitContext& ctx) :
      universe_cfg (load_universe (ctx, "Universe/universe.ini"))
    {
      background_colour = nil;

      auto start_sys = universe_cfg [universe_cfg.start_system ()];
      INILoader ini (ctx.data_rel (start_sys.path));

      INIStatus stat;
      while ((stat = ini.proceed ()) != INIStatus::done)
      {
        if (stat != INIStatus::got_section)
          continue;

        if (ini.section () == "Starfield")
          configure_starfield (ini);
        else if (ini.section () == "Background")
          configure_background (ini);
      }

      player = create_player ();
    }

    ~PlayPhase () = default;

  };

  Phase::Ptr create_play_phase (InitContext& ctx)
  {
    return std::make_unique <PlayPhase> (ctx);
  }

}
