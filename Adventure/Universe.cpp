//
// Adventure
//

#include "Universe.hpp"

#include "INI.hpp"

namespace Ad
{
  void load_system_config (INILoader& ini, std::map <Rk::cstring_ref, SystemCfg, CStringLess>& map)
  {
    SystemCfg sys;

    for (;;)
    {
      auto stat = ini.proceed ();
      if (stat == INIStatus::done_section)
      {
        break;
      }
      else if (stat == INIStatus::got_pair)
      {
        if (ini.key () == "name")
          sys.name = ini.value ();
        else if (ini.key () == "path")
          sys.path = ini.value ();
      }
    }

    if (!sys.name)
      throw std::runtime_error ("Unnamed system");
    else if (!sys.path)
      throw std::runtime_error ("No path for system");

    map.insert (std::make_pair (sys.name, sys));
  }

  void load_dev_config (INILoader& ini, Rk::cstring_ref& start)
  {
    for (;;)
    {
      auto stat = ini.proceed ();
      if (stat == INIStatus::done_section)
      {
        break;
      }
      else if (stat == INIStatus::got_pair)
      {
        if (ini.key () == "start")
          start = ini.value ();
      }
    }
  }

  UniverseCfg load_universe (InitContext& ctx, Rk::cstring_ref path)
  {
    INILoader ini (ctx.data_rel (path));

    std::map <Rk::cstring_ref, SystemCfg, CStringLess> systems;
    Rk::cstring_ref start;

    for (;;)
    {
      auto stat = ini.proceed ();
      if (stat == INIStatus::done)
      {
        break;
      }
      else if (stat == INIStatus::got_section)
      {
        if (ini.section () == "System")
          load_system_config (ini, systems);
        else if (ini.section () == "Dev")
          load_dev_config (ini, start);
      }
    }

    if (systems.empty ())
      throw std::runtime_error ("Empty universe");

    if (!start)
      start = systems.begin ()->first;

    return UniverseCfg (ini.release_buffer (), std::move (systems), start);
  }

}