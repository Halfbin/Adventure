//
// Adventure
//

#pragma once

#include <vector>

#include "Geom.hpp"
#include "Mesh.hpp"

namespace Ad
{
  struct Model
  {
    std::vector <Geom> geoms_;
    std::vector <Mesh> meshes_;

  public:
    Model () = default;

    Model (std::vector <Geom> new_geoms, std::vector <Mesh> new_meshes) :
      geoms_  (std::move (new_geoms)),
      meshes_ (std::move (new_meshes))
    { }

    Model (const Model&) = delete;
    Model& operator = (const Model&) = delete;

    Model (Model&& other) :
      geoms_  (std::move (other.geoms_)),
      meshes_ (std::move (other.meshes_))
    { }

    Model& operator = (Model&& other)
    {
      geoms_  = std::move (other.geoms_);
      meshes_ = std::move (other.meshes_);
      return *this;
    }

    size_t geom_count () const
    {
      return geoms_.size ();
    }

    const Geom* geoms_begin () const
    {
      return geoms_.data ();
    }

    const Geom* geoms_end () const
    {
      return geoms_begin () + geom_count ();
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
