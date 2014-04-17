//
// Adventure
//

#include <Rk/Memory.hpp>
#include <Rk/Vector.hpp>
#include <Rk/Types.hpp>

#include <stdexcept>
#include <memory>
#include <vector>

namespace Ad
{
  struct Poly
  {
    typedef std::unique_ptr <Poly> Ptr;

    enum { max_verts = 10 };
    uint  n_verts;
    v3f   verts [max_verts];

    Poly () :
      n_verts (0)
    { }

  };

  struct Node;

  struct Portal
  {
    Poly shape;
    Map* front;
    Map* back;

    Portal () :
      front (nullptr),
      back  (nullptr)
    { }

  };

  struct Plane
  {
    v3f   normal;
    float dist;
  };

  const Plane plane_invalid = { nil, 0 };

  enum class Side
  {
    front, back, thru, in
  };

  typedef std::vector <Poly> Polys;
  typedef std::vector <Portal*> Portals;

  struct Map
  {
    Node*   owner;
    Plane   plane;
    Polys   polys;
    Portals portals;
    Side    in_side; // front or back for bounding planes

    Map* next;
    Map* prev;

    Map () :
      owner   (nullptr),
      plane   (plane_invalid),
      in_side (Side::in),
      next    (nullptr),
      prev    (nullptr)
    { }

    void remove ()
    {
      if (prev)
      {
        prev->next = next;
        prev = nullptr;
      }

      if (next)
      {
        next->prev = prev;
        next = nullptr;
      }
    }

    ~Map ()
    {
      auto nx = next;
      remove ();
      delete nx;

      for (auto& portal : portals)
      {
        if (in_side == Side::front)
        {
          portal->front = nullptr;
          if (!portal->back)
            delete portal;
        }
        else if (in_side == Side::back)
        {
          portal->back = nullptr;
          if (!portal->front)
            delete portal;
        }
      }
    }

  };

  void add_poly (Map& map, Poly poly)
  {
    map.polys.push_back (poly);
  }

  enum class Contents { non_leaf, space, outside, solid };

  struct Node
  {
    typedef std::unique_ptr <Node> Ptr;

    Plane    split;
    Contents contents;
    Map*     maps;
    Ptr      front,
             back;

    Node () :
      split    (plane_invalid),
      contents (Contents::non_leaf),
      maps     (nullptr)
    { }

    ~Node ()
    {
      delete maps;
    }

  };

  float plane_dist (Plane plane, v3f p)
  {
    return dot (p, plane.normal) - plane.dist;
  }

  Side plane_side (Plane plane, v3f p)
  {
    auto dist = plane_dist (plane, p);
    if (dist < -0.00001)
      return Side::back;
    else if (dist > 0.00001)
      return Side::front;
    else
      return Side::in;
  }

  Side plane_side (Plane plane, Plane other)
  {
    auto dp = dot (plane.normal, other.normal);
    if (abs (dp) <= 0.9999)
      return Side::thru;
    auto dist = other.dist * dp - plane.dist;
    if (dist > 0.00001)
      return Side::front;
    else if (dist < -0.00001)
      return Side::back;
    return Side::in;
  }
  
  bool degenerate (const Poly& p)
  {
    return p.n_verts < 3;
  }

  Side plane_side (Plane plane, const Poly& poly)
  {
    if (degenerate (poly))
      throw std::runtime_error ("plane_side: degenerate polygon");

    Side prev_side = Side::in;

    for (int i = 0; i != poly.n_verts; i++)
    {
      auto vert = poly.verts [i];
      auto side = plane_side (plane, vert);

      if (side != prev_side && side != Side::in && prev_side != Side::in)
        return Side::thru;

      prev_side = side;
    }

    return prev_side;
  }

  bool degenerate (Plane p)
  {
    return abs (p.normal) < 0.00001;
  }

  void add_vert (Poly& poly, v3f v)
  {
    if (poly.n_verts == poly.max_verts)
      throw std::length_error ("add_vert: too many vertices");
    poly.verts [poly.n_verts++] = v;
  }

  v3f plane_intercept (Plane p, v3f a, v3f b)
  {
    auto v = b - a;
    auto div = dot (p.normal, v);
    if (abs (div) < 0.00001)
      throw std::runtime_error ("plane_intercept: line segment parallel to plane");
    float lambda = (p.dist - dot (p.normal, a)) / div;
    return a + v * lambda;
  }

  Side split_poly (const Poly& in, Plane split, Poly& front, Poly& back)
  {
    front.n_verts = 0;
    back.n_verts  = 0;

    if (degenerate (in))
      throw std::runtime_error ("split_poly: degenerate polygon");;

    Side sides [Poly::max_verts];
    for (int i = 0; i != in.n_verts; i++)
      sides [i] = plane_side (split, in.verts [i]);

    auto prev = in.n_verts - 1;
    
    for (int i = 0; i != in.n_verts; i++)
    {
      auto side = sides [i];

      if (side == Side::front)
      {
        if (sides [prev] == Side::in)
          add_vert (front, in.verts [prev]);
        else if (sides [prev] == Side::back)
        {
          auto ixept = plane_intercept (split, in.verts [prev], in.verts [i]);
          add_vert (back, ixept);
          add_vert (front, ixept);
        }

        add_vert (front, in.verts [i]);
      }
      else if (side == Side::back)
      {
        if (sides [prev] == Side::in)
          add_vert (back, in.verts [prev]);
        else if (sides [prev] == Side::front)
        {
          auto ixept = plane_intercept (split, in.verts [prev], in.verts [i]);
          add_vert (front, ixept);
          add_vert (back, ixept);
        }

        add_vert (back, in.verts [i]);
      }
      else
      {
        if (sides [prev] == Side::back)
          add_vert (back, in.verts [i]);
        else if (sides [prev] == Side::front)
          add_vert (front, in.verts [i]);
      }

      prev = i;
    }

    if (front.n_verts == 0 && back.n_verts == 0)
      return Side::in;
    else if (front.n_verts == 0)
      return Side::back;
    else if (back.n_verts == 0)
      return Side::front;
    else
      return Side::thru;
  }

  Plane plane_for_poly (const Poly& poly)
  {
    if (degenerate (poly))
      return plane_invalid;

    auto a = poly.verts [0],
         b = poly.verts [1],
         c = poly.verts [2];

    auto n = unit (cross (c - b, a - b));
    auto d = dot (n, b);

    // Don't do this; directed maps let us avoid individually sorting polygons lying in a split plane.
  /*if (d < 0)
    {
      n = -n;
      d = -d;
    }*/

    return Plane { n, d };
  }

  Map* choose_split (Map* maps, Plane& plane)
  {
    Map* best = nullptr;
    uint best_split = ~uint (0);

    for (auto map = maps; map; map = map->next)
    {
      // Boundary planes are not split candidates.
      if (map->in_side != Side::in)
        continue;

      uint split = 0;

      for (auto other = maps; other; other = other->next)
      {
        if (other == map)
          continue;

        auto side = plane_side (map->plane, other->plane);
        if (side != Side::thru)
          continue;

        for (auto& poly : other->polys)
        {
          side = plane_side (map->plane, poly);
          if (side == Side::thru)
            split++;
        }
      }

      if (split < best_split)
      {
        best = map;
        best_split = split;
      }
    }

    if (best)
      plane = best->plane;
    else
      plane = plane_invalid;

    return best;
  }

  // relink_portal
  // If this map is a boundary of its node, re-link it to the child it is intended for.
  void relink_portal (Map& map)
  {
    for (auto* portal : map.portals)
    {
      if (map.in_side == Side::front)
        portal->front = &map;
      else if (map.in_side == Side::back)
        portal->back = &map;
    }
  }

  void add_map (Node& node, Map* map)
  {
    map->remove ();
    map->next = node.maps;
    if (node.maps)
      node.maps->prev = map;
    node.maps = map;
  }

  struct TreeInfo
  {
    int n_polys;
    int n_solid;
    int n_space;
  };

  TreeInfo partition (Node& node)
  {
    // Choose a splitting plane.
    auto split_map = choose_split (node.maps, node.split);

    if (!split_map)
    {
      TreeInfo info;
      info.n_polys = 0;

      // We're a leaf.
      if (node.contents == Contents::solid)
      {
        delete node.maps;
        node.maps = nullptr;
        info.n_solid = 1;
        info.n_space = 0;
      }
      else
      {
        for (auto map = node.maps; map; map = map->next)
          info.n_polys += map->polys.size ();
        info.n_solid = 0;
        info.n_space = 1;
      }

      return info;
    }

    if (split_map == node.maps)
      node.maps = split_map->next;

    // Create children.
    node.contents = Contents::non_leaf;
    node.front = std::make_unique <Node> ();
    node.back  = std::make_unique <Node> ();

    // Link the split portal to the children, and remember it for the complementary map.
    auto split_portal = new Portal;
    split_portal->shape = make_plane_rect (node.split, 10000);
    split_portal->front = split_map;
  //split_portal->back  = node.back.get ();

    // The split map naturally goes in the front.
    split_map->owner = &node;
    split_map->in_side = Side::front;
    split_map->portals.push_back (split_portal);
    add_map (*node.front, split_map);
    //split_map = nullptr;

    // The complementary map, intended for the back.
    Map* split_back = nullptr;

    // Sort or split maps into children as necessary.
    while (node.maps)
    {
      // Grab your morning toast
      auto map = node.maps;
      node.maps = map->next;
      map->remove ();

      // Decide what to put on it.
      auto side = plane_side (node.split, map->plane);

      if (side == Side::front)
      {
        // Parallel plane lies completely in front of split...
        relink_portal (*map, *node.front);
        add_map (*node.front, map);
      }
      else if (side == Side::back)
      {
        // ...or completely behind.
        relink_portal (*map, *node.back);
        add_map (*node.back, map);
      }
      else if (side == Side::in)
      {
        // Sometimes there is a map coplanar to split, but pointing the other way.
        map->in_side = Side::front;
        split_back = map;
      }
      else
      {
        // Make maps for the children.
        auto back_map = new Map;
        back_map->plane = map->plane;
        back_map->in_side = map->in_side;

        auto front_map = new Map;
        front_map->plane = map->plane;
        front_map->in_side = map->in_side;

        for (auto& poly : map->polys)
        {
          auto side = plane_side (node.split, poly);

          if (side == Side::front)
          {
            add_poly (*front_map, poly);
          }
          else if (side == Side::back)
          {
            add_poly (*back_map, poly);
          }
          else if (side == Side::thru)
          {
            Poly front_part, back_part;
            split_poly (poly, node.split, front_part, back_part);
            add_poly (*front_map, front_part);
            add_poly (*back_map, back_part);
          }
        }

        // Now split the portal.
        front_map->portal = new Portal;
        back_map->portal = new Portal;
        auto& front_portal = *front_map->portal;
        auto& back_portal  = *back_map->portal;

        back_portal.front = front_portal.front = map->portal->front;
        back_portal.back  = front_portal.back  = map->portal->back;
        split_poly (map->portal->shape, node.split, front_portal.shape, back_portal.shape);

        if (degenerate (front_portal.shape) || degenerate (back_portal.shape))
          throw std::runtime_error ("Welp");

        // Link portals if necessary.
        relink_portal (*front_map, *node.front);
        relink_portal (*back_map,  *node.back);

        add_map (*node.front, front_map);
        add_map (*node.back, back_map);

        // We also need to clip the new split portal.

        delete map;
      }
    }

    // If we still don't have a complementary map to split, make one.
    if (!split_back)
    {
      split_back = new Map;
      split_back->in_side = Side::back;
      split_back->plane = node.split;
    }

    // Assign contents.
    node.back->contents = Contents::solid;

    if (split_back->polys.empty ())
      node.front->contents = Contents::space;
    else
      node.front->contents = Contents::solid;

    // Add in the complementary split map to back.
    split_back->portal = &split_portal;
    split_portal->back = // oh shit
    add_map (*node.back, split_back);

    // Partition children.
    auto front_info = partition (*node.front);
    auto back_info  = partition (*node.back);

    TreeInfo info;
    info.n_polys = front_info.n_polys + back_info.n_polys;
    info.n_solid = front_info.n_solid + back_info.n_solid;
    info.n_space = front_info.n_space + back_info.n_space;
    return info;
  }

  Map* map_polys (const Poly* polys, uint n_polys)
  {
    Map* maps = nullptr;

    for (uint i = 0; i != n_polys; i++)
    {
      auto poly = polys [i];

      // What plane is it in?
      auto plane = plane_for_poly (poly);

      Map* map = maps;
      while (map)
      {
        auto side = plane_side (map->plane, plane);
        if (side == Side::in)
          break;
        map = map->next;
      }

      // No map for this plane yet; create one.
      if (!map)
      {
        map = new Map;
        map->next = maps;
        if (maps)
          maps->prev = map;
        maps = map;
        map->plane = plane;
      }

      // Add polygon to map.
      add_poly (*map, poly);
    }

    return maps;
  }

  Side plane_side (Plane plane, const Map& map)
  {
    Side side = plane_side (plane, map.plane);

    if (side != Side::thru)
      return side;

    // Map is through ref. Compare on a poly-by-poly basis.
    Side in_side = Side::in;

    for (const Poly& poly : map.polys)
    {
      side = plane_side (plane, poly);

      // Poly straddles plane.
      if (side == Side::thru)
        break;

      // First poly. Everything else has to be on this same side.
      if (in_side == Side::in)
      {
        in_side = side;
      }
      // If the poly was on the other side of plane to the previous ones, map is thru plane.
      else if (side != in_side && side != Side::in)
      {
        side = Side::thru;
        break;
      }
    }

    return side;
  }

  Poly make_plane_rect (Plane plane, float scale)
  {
    v3f n = plane.normal;
    v3f an { abs (n.x), abs (n.y), abs (n.z) };
    auto d = plane.dist;

    // Figure out a basis for the plane.
    v3f s;
    if (an.z > an.y && an.z > an.x)
      s = {1,0,0};
    else
      s = {0,0,1};

    s = unit (cross (n, s));
    v3f t = cross (n, s);
    v3f p = d * n;

    s *= scale; t *= scale;

    Poly rect;
    add_vert (rect, p + s + t);
    add_vert (rect, p - s + t);
    add_vert (rect, p - s - t);
    add_vert (rect, p + s - t);
    return rect;
  }

  // init_maps
  // Finds boundary planes and seeds the first portals.
  void init_maps (Node& root, Node& outside)
  {
    // Find bounding planes
    static const int max_boundaries = 32;
    Map* boundaries [max_boundaries];
    uint n_boundaries = 0;

    for (Map* ref = root.maps; ref; ref = ref->next)
    {
      // Check every other map against our reference plane to see if they all lie on one side.
      Side side = Side::in;

      for (Map* map = root.maps; map; map = map->next)
      {
        // Don't test against ourselves.
        if (map == ref)
          continue;

        side = plane_side (ref->plane, *map);

        // Map straddles ref.
        if (side == Side::thru)
          break;

        // First map. Everything else has to be on this same side.
        if (ref->in_side == Side::in)
        {
          ref->in_side = side;
        }
        // If the map was on the other side of ref to the previous ones, ref is not a boundary.
        else if (side != ref->in_side && side != Side::in)
        {
          side = Side::thru;
          break;
        }
      }

      // Nope.
      if (side == Side::thru)
      {
        ref->in_side = Side::in;
        continue;
      }

      // Degenerate world.
      if (ref->in_side == Side::in || ref->in_side == Side::thru)
        throw std::runtime_error ("find_boundary_planes: degenerate world");

      // Everything turned out to be on one side of ref, so it's a boundary.
      if (n_boundaries == max_boundaries)
        throw std::length_error ("find_boundary_planes: too many boundary planes");
      boundaries [n_boundaries++] = ref;
    }

    // Create initial portals for boundaries.
    for (int i = 0; i != n_boundaries; i++)
    {
      auto* boundary = boundaries [i];
      auto portal = new Portal;
      portal->shape = make_plane_rect (boundary->plane, 10000);
      boundary->portals.push_back (new Portal);

      for (int j = 0; j != n_boundaries; j++)
      {
        if (i == j)
          continue;

        auto clip = boundaries [j]->plane;

        auto side = plane_side (boundary->plane, clip);
        if (side != Side::thru)
          continue;

        Poly front, back;
        split_poly (portal->shape, clip, front, back);

        if (boundary->in_side == Side::front)
          portal->shape = front;
        else
          portal->shape = back;
      }
    }
  }

  struct Tree
  {
    TreeInfo  info;
    Node::Ptr root;
  };

  Tree compile (const Poly* polys, uint n_polys)
  {
    // Sort polys into planes for easier handling.
    auto maps = map_polys (polys, n_polys);

    // Make our root node.
    auto root = std::make_unique <Node> ();
    root -> maps = std::move (maps);

    // Set up initial portals and boundaries.
    Node outside;
    init_maps (*root, outside);

    // Partition the root recursively until we have convex leaves.
    auto info = partition (*root);

    // TODO: rest of the process

    Tree tree;
    tree.info = info;
    tree.root = std::move (root);
    return tree;
  }

  struct Surface
  {
    Plane plane;
    Poly  poly;
  };

  struct Brush
  {
    enum { max_surfs = 32 };
    Surface surfs [max_surfs];
    uint n_surfs;

    Brush () :
      n_surfs (0)
    { }

  };

  void add_surface (Brush& brush, Plane plane)
  {
    brush.surfs [brush.n_surfs++] = { plane };
  }

  // wind_brush
  // Generates face windings for brush. Any existing polys will be replaced.
  void wind_brush (Brush& brush)
  {
    for (int i = 0; i != brush.n_surfs; i++)
      brush.surfs [i].poly = make_plane_rect (brush.surfs [i].plane, 10000);

    for (int i = 0; i != brush.n_surfs; i++)
    {
      auto& surf = brush.surfs [i];

      for (int j = 0; j != brush.n_surfs; j++)
      {
        // No point clipping with our own plane.
        if (i == j)
          continue;

        Plane clip = brush.surfs [j].plane;

        // No point clipping by parallel planes.
        auto side = plane_side (surf.plane, clip);
        if (side != Side::thru)
          continue;

        // Use the plane to chop the winding.
        Poly front, back;
        split_poly (surf.poly, clip, front, back);
        
        // Keep the half on the inside.
        surf.poly = std::move (back);

        if (degenerate (surf.poly))
          throw std::runtime_error ("wind_brush: vanishing face; may be an invalid brush");
      }
    }
  }

  Brush make_axial (v3f mins, v3f maxs)
  {
    Brush b;
    add_surface (b, Plane { v3f { -1,  0,  0 }, -mins.x });
    add_surface (b, Plane { v3f {  1,  0,  0 },  maxs.x });
    add_surface (b, Plane { v3f {  0, -1,  0 }, -mins.y });
    add_surface (b, Plane { v3f {  0,  1,  0 },  maxs.y });
    add_surface (b, Plane { v3f {  0,  0, -1 }, -mins.z });
    add_surface (b, Plane { v3f {  0,  0,  1 },  maxs.z });
    wind_brush (b);
    return b;
  }

  std::vector <Poly> make_polys (Brush* brushes, uint n_brushes)
  {
    std::vector <Poly> polys;

    for (int i = 0; i != n_brushes; i++)
    {
      auto& brush = brushes [i];

      for (int j = 0; j != brush.n_surfs; j++)
        polys.push_back (brush.surfs [j].poly);
    }

    return polys;
  }

  extern "C" int main () try
  {
    Brush brushes [6] = {
      make_axial ({-10, -10, -10}, { 10,  10, -9}),
      make_axial ({-10, -10,   9}, { 10,  10, 10}),
      make_axial ({-10, -10,  -9}, { 10,  -9,  9}),
      make_axial ({-10,   9,  -9}, { 10,  10,  9}),
      make_axial ({-10,  -9,  -9}, { -9,   9,  9}),
      make_axial ({  9,  -9,  -9}, { 10,   9,  9}),
    };

    auto polys = make_polys (brushes, 6);
    auto tree = compile (polys.data (), polys.size ());

    return 0;
  }
  catch (...)
  {
    return 1;
  }

}
