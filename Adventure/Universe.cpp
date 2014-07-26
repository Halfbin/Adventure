//
// Adventure
//

#include "Universe.hpp"

#include "INI.hpp"

namespace Ad
{
  void load_system_config (INILoader& ini, std::map <Rk::cstring_ref, SystemCfg, CStringRefLess>& map)
  {
    SystemCfg sys;

    INIStatus stat;
    while ((stat = ini.proceed ()) != INIStatus::done_section)
    {
      if (stat != INIStatus::got_pair)
        continue;

      if (ini.key () == "name")
        sys.name = ini.value ();
      else if (ini.key () == "path")
        sys.path = ini.value ();
    }

    if (!sys.name)
      throw std::runtime_error ("Unnamed system");
    else if (!sys.path)
      throw std::runtime_error ("No path for system");

    map.insert (std::make_pair (sys.name, sys));
  }

  void load_dev_config (INILoader& ini, Rk::cstring_ref& start, Rk::cstring_ref& ship)
  {
    INIStatus stat;
    while ((stat = ini.proceed ()) != INIStatus::done_section)
    {
      if (stat != INIStatus::got_pair)
        continue;

      if (ini.key () == "start")
        start = ini.value ();
      else if (ini.key () == "ship")
        ship = ini.value ();
    }
  }

  UniverseCfg load_universe (InitContext& ctx, Rk::cstring_ref path)
  {
    INILoader ini (ctx.data_rel (path));

    std::map <Rk::cstring_ref, SystemCfg, CStringRefLess> systems;
    Rk::cstring_ref start, ship;

    INIStatus stat;
    while ((stat = ini.proceed ()) != INIStatus::done)
    {
      if (stat != INIStatus::got_section)
        continue;

      if (ini.section () == "System")
        load_system_config (ini, systems);
      else if (ini.section () == "Dev")
        load_dev_config (ini, start, ship);
    }

    if (systems.empty ())
      throw std::runtime_error ("Empty universe");

    if (!start)
      start = systems.begin ()->first;

    if (!ship)
      throw std::runtime_error ("No starting ship");

    return UniverseCfg (ini.release_buffer (), std::move (systems), start, ship);
  }

}