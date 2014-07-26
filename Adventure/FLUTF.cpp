//
// Adventure
//

#include "FLUTF.hpp"

namespace Ad
{
  UTFNode::UTFNode (const u8* node, const UTFNode* par, const u8* ns, const u8* ss, const u8* ds) :
    parent (par),
    ns     (ns),
    ss     (ss),
    ds     (ds)
  {
    auto next_off = *(const u32*) (node +  0);;
    if (next_off)
      next_ptr = ns + next_off;
    else
      next_ptr = nullptr;

    auto p_name = ss + *(const u32*) (node +  4);

    data_len = *(const u32*) (node + 24);

    if (data_len)
      child_or_data = ds + *(const u32*) (node + 16);
    else
      child_or_data = ns + *(const u32*) (node + 16);

    node_name = Rk::cstring_ref ((const char*) p_name);
  }

  UTFNode UTFNode::next () const
  {
    if (!has_next ())
      return UTFNode ();
    return UTFNode (next_ptr, parent, ns, ss, ds);
  }

  UTFNode UTFNode::child () const
  {
    if (!has_child ())
      return UTFNode ();
    return UTFNode (child_or_data, this, ns, ss, ds);
  }

  const u8* UTFNode::data () const
  {
    if (has_child ())
      throw std::logic_error ("UTFNode has no data - non-leaf node");
    return child_or_data;
  }

  UTFNode load_utf (const u8* file, const size_t length)
  {
    if (length < 28)
      throw std::runtime_error ("Source file too short");

    if (!std::equal (file, file + 4, (const u8*) "UTF "))
      throw std::runtime_error ("Source file has incorrect signature");

    if (*(const u32*) (file + 4) != 0x0101)
      throw std::runtime_error ("Source file has unsupported version");

    auto nodes   = file + *(const u32*) (file +  8);
    auto strings = file + *(const u32*) (file + 24);
    auto data    = file + *(const u32*) (file + 36);

    return UTFNode (nodes, nullptr, nodes, strings, data);
  }

}
