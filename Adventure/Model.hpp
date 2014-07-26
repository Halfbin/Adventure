//
// Adventure
//

#pragma once

#include "Frame.hpp"

namespace Ad
{
  struct Model
  {
    Geom               geom_;
    std::vector <Mesh> meshes_;

  public:
    Model () = default;

    Model (Geom new_geom, std::vector <Mesh> new_meshes) :
      geom_   (std::move (new_geom)),
      meshes_ (std::move (new_meshes))
    { }

    Model (const Model&) = delete;
    Model& operator = (const Model&) = delete;

    Model (Model&& other) :
      geom_   (std::move (other.geom_)),
      meshes_ (std::move (other.meshes_))
    { }

    Model& operator = (Model&& other)
    {
      geom_   = std::move (other.geom_);
      meshes_ = std::move (other.meshes_);
      return *this;
    }

    const Geom& geom () const
    {
      return geom_;
    }

    size_t mesh_count () const
    {
      return meshes_.size ();
    }

    const Mesh* meshes_begin () const
    {
      return meshes_.data ();
    }

    const Mesh* meshes_end () const
    {
      return meshes_begin () + mesh_count ();
    }

  };

}
