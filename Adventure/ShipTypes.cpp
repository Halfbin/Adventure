//
// Adventure
//

#include "ShipTypes.hpp"

#include "INI.hpp"

namespace Ad
{
  void load_ship_type (INILoader& ini, std::map <Rk::cstring_ref, ShipType, CStringRefLess>& map)
  {
    ShipType st;

    INIStatus stat;
    while ((stat = ini.proceed ()) != INIStatus::done_section)
    {
      if (stat != INIStatus::got_pair)
        continue;

      if (ini.key () == "name")
        st.name = ini.value ();
      else if (ini.key () == "model")
        st.model_path = ini.value ();
      else if (ini.key () == "materials")
        st.materials_path = ini.value ();
    }

    if (!st.name)
      throw std::runtime_error ("Unnamed ship type");
    else if (!st.model_path)
      throw std::runtime_error ("No model for ship type");
    else if (!st.materials_path)
      throw std::runtime_error ("No materials for ship type");

    map.insert (std::make_pair (st.name, st));
  }

  ShipTypes load_ship_types (InitContext& ctx, Rk::cstring_ref path)
  {
    INILoader ini (ctx.data_rel (path));

    std::map <Rk::cstring_ref, ShipType, CStringRefLess> ship_types;
    Rk::cstring_ref start;

    INIStatus stat;
    while ((stat = ini.proceed ()) != INIStatus::done)
    {
      if (stat != INIStatus::got_section)
        continue;

      if (ini.section () == "Ship")
        load_ship_type (ini, ship_types);
    }

    if (ship_types.empty ())
      throw std::runtime_error ("No ship types");

    return ShipTypes (ini.release_buffer (), std::move (ship_types));
  }

}
