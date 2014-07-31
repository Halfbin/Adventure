//
// Adventure
//

#include "Phase.hpp"

#include "ModelShader.hpp"
#include "ShipTypes.hpp"
#include "Starfield.hpp"
#include "Universe.hpp"
#include "Texture.hpp"
#include "Buffer.hpp"
#include "Player.hpp"
#include "Solar.hpp"
#include "Geom.hpp"
#include "Grab.hpp"
#include "INI.hpp"

#include <iomanip>
#include <sstream>

#include <Rk/transform.hpp>

#include <GL/glew.h>

namespace Ad
{
  namespace
  {
    template <typename map_t, typename key_t, typename def_t>
    auto find_or (const map_t& map, key_t&& key, def_t&& def)
      -> typename map_t::mapped_type
    {
      auto iter = map.find (std::forward <key_t> (key));
      if (iter == map.end ())
        return std::forward <def_t> (def);
      return iter->second;
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
    ShipTypes ship_types;
    std::map <Rk::cstring_ref, SolarType*, CStringRefLess> solar_types;

    struct Point
    {
      v3i field;
      v3f pos;
    };

    std::map <std::string, Point> points;

    Player::Ptr player;
    std::vector <Solar> solars;
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
        else if (is_button_up (ev, Buttons::left))
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
        ent->advance (time, step);
        collide.add_ent (ent.get ());
      }

      player->advance (time, step);
      collide.add_ent (player.get ());

      for (auto ev : collide.find_collisions ())
        ev.ent->collide (ev.normal, ev.pdist);
    }

    void render (Frame& frame)
    {
      frame.clear_colour = background_colour;
      frame.set_starfield (starfield);

      for (auto&& solar : solars)
        solar.draw (frame);

      for (auto&& ent : entities)
        ent->draw (frame);

      player->draw (frame);
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

    SolarFactory::Ptr make_solar_factory (Rk::cstring_ref type_name)
    {
      auto type = find_or (solar_types, type_name, nullptr);
      if (!type)
        return nullptr;
      return type->make_factory ();
    }

    void configure_solar (INILoader& ini)
    {
      std::vector <std::pair <Rk::cstring_ref, Rk::cstring_ref>> params;
      SolarFactory::Ptr factory;

      INIStatus stat;
      while ((stat = ini.proceed ()) == INIStatus::got_pair)
      {
        if (ini.key () == "type" && !factory)
          factory = make_solar_factory (ini.value ());
        else
          params.push_back (std::make_pair (ini.key (), ini.value ()));
      }

      if (!factory)
        throw std::runtime_error ("Solar with no type");

      for (const auto param : params)
        factory->param (param.first, param.second);

      solars.push_back (factory->create ());
    }

    void configure_point (INILoader& ini)
    {
      std::string name;
      v3i field = {0,0,0};
      v3f pos = {0,0,0};

      INIStatus stat;
      while ((stat = ini.proceed ()) == INIStatus::got_pair)
      {
        if (ini.key () == "name")
          name = to_string (ini.value ());
        else if (ini.key () == "field")
          grab_v3i (field, ini.value ());
        else if (ini.key () == "pos")
          grab_v3f (pos, ini.value ());
      }

      points [name] = Point { field, pos };
    }

  public:
    PlayPhase (InitContext& ctx) :
      universe_cfg (load_universe (ctx, "Universe/universe.ini")),
      ship_types (load_ship_types (ctx, "Ships/ShipTypes.ini"))
    {
      for (auto ty = SolarType::head (); ty; ty = ty->next ())
        solar_types.insert (std::make_pair (ty->name (), ty));

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
        else if (ini.section () == "Solar")
          configure_solar (ini);
        else if (ini.section () == "Point")
          configure_point (ini);
      }

      auto start_ship = ship_types [universe_cfg.start_ship_type ()];

      Point player_pos = { nil, nil };
      auto iter = points.find ("spawn");
      if (iter != points.end ())
        player_pos = iter->second;

      player = create_player (ctx, start_ship, player_pos.field, player_pos.pos, identity);
    }

    ~PlayPhase () = default;

  };

  Phase::Ptr create_play_phase (InitContext& ctx)
  {
    return std::make_unique <PlayPhase> (ctx);
  }

}
