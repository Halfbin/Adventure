//
// Adventure
//

#include "Phase.hpp"

#include "ModelShader.hpp"
#include "Starfield.hpp"
#include "Texture.hpp"
#include "Buffer.hpp"
#include "Player.hpp"
#include "Geom.hpp"
#include "Item.hpp"

#include <Rk/transform.hpp>

#include <GL/glew.h>

namespace Ad
{
  extern "C" __declspec (dllimport) void __stdcall OutputDebugStringA (const char*);

/*Buffer make_floor_data ()
  {
    float data [] = {
      -20.f, -20.f, 0.f,   0,  0, // 0
       20.f, -20.f, 0.f,  10,  0, // 1
       20.f,  20.f, 0.f,  10, 10, // 2
      -20.f,  20.f, 0.f,   0, 10, // 3
       40.f, -20.f, 15.f, 16.25f,  0, // 4
       40.f,  20.f, 15.f, 16.25f, 10, // 5
    };

    return Buffer (sizeof (data), data);
  }

  Buffer make_floor_idxs ()
  {
    u8 idxs [] = { 0, 1, 2, 0, 2, 3, 1, 4, 5, 1, 5, 2 };
    return Buffer (sizeof (idxs), idxs);
  }

  Geom make_floor ()
  {
    auto data = make_floor_data ();
    auto idxs = make_floor_idxs ();

    Attrib attribs [2] = {
      { ModelShader::attrib_vertpos, data.name (), 3, GL_FLOAT, 20,  0 },
      { ModelShader::attrib_tcoords, data.name (), 2, GL_FLOAT, 20, 12 }
    };

    auto geom = Geom (attribs, 2, idxs.name (), 12, GL_UNSIGNED_BYTE);

    data.release ();
    idxs.release ();

    return geom;
  }*/

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
    Player::Ptr player;
    std::vector <Entity::Ptr> entities;
    CollisionState collide;

  /*Geom         floor;
    Texture::Ptr floor_tex;*/

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
      frame.clear_colour = { 0.00f, 0.01f, 0.025f, 1.00f };
      frame.set_starfield (starfield);

      // frame.draw (floor, 0, floor.index_count (), floor_tex -> name (), {0,0,0,1}, nil, identity);

      for (auto&& ent : entities)
        ent -> draw (frame);

      player -> draw (frame);
    }

  public:
    PlayPhase ()/* :
      floor (make_floor ())*/
    {
      starfield = make_starfield (20000);

      player = create_player ();
    /*add_entity (create_item ("Art/Globe1a.png", {-5,-5, 0}));
      add_entity (create_item ("Art/Globe1a.png", { 5,-5, 0}));
      add_entity (create_item ("Art/Globe1a.png", { 5, 5, 0}));
      add_entity (create_item ("Art/Globe1a.png", {-5, 5, 0}));

      floor_tex = Texture::create ("Art/Flags1a.png", TexFlags::nearest);

      collide.add_plane ({ 0, 0, 1},   0);
      collide.add_plane ({ 1, 0, 0}, -20);
      collide.add_plane ({-1, 0, 0}, -40);
      collide.add_plane ({ 0, 1, 0}, -20);
      collide.add_plane ({ 0,-1, 0}, -20);

      collide.add_plane ({-0.6f, 0, 0.8f}, -12);*/
    }

    ~PlayPhase () = default;

  };

  Phase::Ptr create_play_phase ()
  {
    return std::make_unique <PlayPhase> ();
  }

}
