//
// Adventure
//

#include "Phase.hpp"

#include "ModelShader.hpp"
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

  Buffer make_floor_data ()
  {
    float data [] = {
      -20.f, -20.f, 0.f,  0,  0,
       20.f, -20.f, 0.f, 10,  0,
       20.f,  20.f, 0.f, 10, 10,
      -20.f,  20.f, 0.f,  0, 10
    };

    return Buffer (sizeof (data), data);
  }

  Buffer make_floor_idxs ()
  {
    u8 idxs [6] = { 0, 1, 2, 0, 2, 3 };
    return Buffer (sizeof (idxs), idxs);
  }

  Geom make_floor (const Buffer& data, const Buffer& idxs)
  {
    Attrib attribs [2] = {
      { ModelShader::attrib_vertpos, data.name (), 3, GL_FLOAT, 20,  0 },
      { ModelShader::attrib_tcoords, data.name (), 2, GL_FLOAT, 20, 12 }
    };

    return Geom (attribs, 2, idxs.name ());
  }

  class PlayPhase :
    public Phase
  {
    Player::Ptr player;
    std::vector <Entity::Ptr> entities;

    Buffer floor_data,
           floor_idxs;
    Geom   floor;
    Texture::Ptr floor_tex;

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
        {
          OutputDebugStringA ("Buttons::left\n");
          ctx.frontend.enter_mouse_look ();
        }
        else if (is_key_up (ev, Keys::escape))
        {
          OutputDebugStringA ("Keys::escape\n");
          ctx.frontend.leave_mouse_look ();
        }
      }

      player -> input (ctx);
    }

    void tick (float time, float step)
    {
      t0 = time;
      t1 = time + step;

      for (auto&& ent : entities)
        ent -> tick (time, step);

      player -> tick (time, step);
    }

    void render (Frame& frame)
    {
      frame.clear_colour = { 0.00f, 0.26f, 0.51f, 1.00f };

      frame.draw (floor.name (), 6, GL_UNSIGNED_BYTE, floor_tex -> name (), {0,0,0,1}, nil, identity);

      for (auto&& ent : entities)
        ent -> draw (frame);

      player -> draw (frame);
    }

  public:
    PlayPhase () :
      floor_data (make_floor_data ()),
      floor_idxs (make_floor_idxs ()),
      floor (make_floor (floor_data, floor_idxs))
    {
      player = create_player ();
      add_entity (create_item ("Art/Globe1a.png", v2i (-5, -5)));
      add_entity (create_item ("Art/Globe1a.png", v2i ( 5, -5)));
      add_entity (create_item ("Art/Globe1a.png", v2i ( 5,  5)));
      add_entity (create_item ("Art/Globe1a.png", v2i (-5,  5)));

      floor_tex = Texture::create ("Art/Flags1a.png", TexFlags::nearest);
    }

    ~PlayPhase () = default;

  };

  Phase::Ptr create_play_phase ()
  {
    return std::make_unique <PlayPhase> ();
  }

}
