#include <Windows.h>

BOOL APIENTRY DllMain (HMODULE, DWORD, LPVOID) { return TRUE; }

#include <easyhook.h>
#include <string>
#include <iostream>
#include <Psapi.h>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <fstream>
#include <map>
#include <sstream>

#include "asm_x64.hpp"
#include "patching.hpp"
#include "common_types.hpp"
#include "blz_stl.hpp"
#include "renderservice.hpp"

struct callback_test_t
{
  void operator() (RenderRequest::RequestData* request_data, route_arg_3_t* arg3) const
  {
    arg3->message_set_status (204);
    arg3->message_send_and_release();
  }
};
blz::static_function<callback_test_t, void, RenderRequest::RequestData*, route_arg_3_t*> const callback_test;

#pragma pack(push, 1)
using _BYTE = unsigned char;
struct struc_6
{
  const char *name;
  int num_fields_in_file;
  int record_size;
  int num_fields;
  int id_column;
  char field_18;
  _BYTE gap19[7];
  int *field_offsets;
  int *field_sizes;
  int *field_types;
  int *field_flags;
  int *field_sizes_in_file;
  int *field_types_in_file;
  int *field_flags_in_file;
  int field_58;
  int table_hash;
  _BYTE gap60[4];
  int layout_hash;
  int field_68;
  int field_6C;
  int field_70;
  _BYTE gap74[4];
  _UNKNOWN *field_78;
  _BYTE gap80[8];
  int field_88;
  int field_8C;
  int some_special_column;
  _BYTE gap94[4];
  __int64 sort_func;
  const char *table_name;
  const char **field_names_in_file;
  const char **field_names;
  const char *fk_clause;
  char field_C0;
  char field_C1;
};
#pragma pack(pop)
static_assert(sizeof(struc_6) == 0xc2, "");

struct wowdbclient {
  void* vtable;
  using info_t = struc_6;
  info_t* info;
};
//! \note hardcoded to 8.0.1.26095
fun<bool (wowdbclient*, char const*, char const*, unsigned int)> db_load_with_dbgenfallback
  = 0x1401F7A30;
fun<bool (wowdbclient*, unsigned int)> db_load_with_dbgenfallback_no_override
  = 0x1401F7B20;
fun<wowdbclient* (wowdbclient*, wowdbclient::info_t*)> sub_1401F3480 = 0x1401F3480;
fun<bool()> renderservice_main = 0x14005D670;

std::string ea(void const* addr)
{
  auto unrebased = unrebase (addr);
  std::stringstream ss; ss << "0x" << std::hex << unrebased;
  return ss.str();
}

std::string make_type(void const* addr, std::string type)
{
  return "MakeUnkn(" + ea(addr) + ", 1);SetType(" + ea(addr) + ", \"" + type + "\");";
}

std::string type_to_str (int t, int f)
try
{
  switch (t)
  {
    case 0:
      switch (f)
      {
        case 0|0|0: 
          return " int0?00"; // ???
        case 0|0|1: 
          return " int0?01"; // ???
        case 0|2|0: 
          return " int0?20"; // ???
        case 0|2|1: 
          return " int0?21"; // ???
        case 4|0|0:
          return "uint0?00"; // ???
        case 4|0|1:
          return "uint0?01"; // ???
        case 4|2|0:
          return "uint0?20"; // ???
        case 4|2|1:
          return "uint0?21"; // ???
        default:
          throw std::logic_error ("f != ???");
      }
    case 4:
      switch (f)
      {
        case 0|0|0: 
          return " int4?00"; // ???
        case 0|2|0: 
          return " int4?20"; // ???
        case 4|0|0: 
          return "uint4?00"; // ???
        case 4|2|0: 
          return "uint4?20"; // ???
        default:
          throw std::logic_error ("f != ???");
      }
    case 5:
      switch (f)
      {
        case 0|0|0: 
          return " int5?00"; // ???
        case 0|0|1: 
          return " int5?01"; // ???
        case 0|2|0: 
          return " int5?20"; // ???
        case 4|0|0: 
          return "uint5?00"; // ???
        case 4|0|1: 
          return "uint5?01"; // ???
        case 4|2|0: 
          return "uint5?20"; // ???
        case 4|2|1: 
          return "uint5?21"; // ???
        default:
          throw std::logic_error ("f != ???");
      }
    case 1:
      switch (f)
      {
        case 0|0|0: 
          return " int1?00"; // ???
        default:
          throw std::logic_error ("f != ???");
      }
    case 2:
      switch (f)
      {
        case 0:
          return "string";
        case 8:
          return "locstring";
        default:
          throw std::logic_error ("f != 0/8");
      }
    case 3:
      switch (f)
      {
        case 0:
          return "float";
        case 2:
          return "float?2"; // ???
        default:
          throw std::logic_error ("f != 0");
      }
    default:
      throw std::logic_error ("unknown type");
  }
}
catch (std::exception const& ex)
{
  std::cerr << "type_to_str (" << t << ", " << f << ") = " << ex.what() << "\n";
  throw;
}

void dump_db_info_ida (wowdbclient* a)
{
  std::string name = a->info->name;
  std::string ida_cmd;
  ida_cmd += "MakeName(" + ea (a) + ", \"db_" + a->info->name + "\");";
  ida_cmd += "MakeName(" + ea (a->info) + ", \"__ZN" + std::to_string(name.size()) + name + "6s_metaE\");";
  ida_cmd += make_type (a->info, "struc_6");
  ida_cmd += make_type (a->info->table_name, "char const a[]");
  ida_cmd += make_type (a->info->field_names_in_file, "char const* a[" + std::to_string (a->info->num_fields_in_file) + "]");
  ida_cmd += make_type (a->info->field_names, "char const* a[" + std::to_string (a->info->num_fields) + "]");
  std::ofstream("E:/git/renderserver/tmp.ida.py", std::ios_base::app) << ida_cmd << "\n";
  
  std::ostringstream dbdss;
  dbdss << "COLUMNS\n";
  
  std::map<std::string, std::pair<int /*type*/, int /*flags*/>> names;
  
  for (int i = 0; i < a->info->num_fields_in_file; ++i) {
    auto p = std::make_pair (a->info->field_types_in_file[i], a->info->field_flags_in_file[i]);
    if ( !names.emplace (a->info->field_names_in_file[i], p).second
      && names.at (a->info->field_names_in_file[i]) != p
        )
      throw std::logic_error ("different type for column in file and memory");
  }
  for (int i = 0; i < a->info->num_fields; ++i) {
    auto p = std::make_pair (a->info->field_types[i], a->info->field_flags[i]);
    if ( !names.emplace (a->info->field_names[i], p).second
      && names.at (a->info->field_names[i]) != p
        )
      throw std::logic_error ("different type for column in file and memory");
  }
  
  

  for (auto const& name : names)
    dbdss << type_to_str(name.second.first, name.second.second) << " " << name.first << "\n";
  std::ofstream("E:/git/renderserver/tmp-" + name + ".dbd") << dbdss.str() << "\n";
}

wowdbclient *__fastcall sub_1401F3480_hook(wowdbclient *db, wowdbclient::info_t *info)
{
  //std::cerr << "ctor " << info->name << "\n";
  sub_1401F3480 (db, info);
  dump_db_info_ida (db);
  return db;
}

void on_inject()
{
  std::ofstream("E:/git/renderserver/tmp.ida.py") << "\n";
  hook (sub_1401F3480, sub_1401F3480_hook);
  
  hook ( db_load_with_dbgenfallback
       , [] (wowdbclient* a, char const* b, char const* c, unsigned int d)
         {
           if (std::string (a->info->name) == "AnimationNames") return true;
           if (std::string (a->info->name) == "FileDataComplete") return true;
           if (std::string (a->info->name) == "FilePaths") return true;
           return db_load_with_dbgenfallback (a, b, c, d);
         }
       );
  hook ( db_load_with_dbgenfallback_no_override
       , [] (wowdbclient* a, unsigned int b)
         {
           if (std::string (a->info->name) == "AnimationNames") return true;
           if (std::string (a->info->name) == "FileDataComplete") return true;
           if (std::string (a->info->name) == "FilePaths") return true;
           return db_load_with_dbgenfallback_no_override (a, b);
         }
       );

       
  hook ( CVarRegister
       , [] (char const* name, char const* a, unsigned int b, char const* c, bool (*d)(CVar*, char const*, char const*, void *), unsigned int e, bool f, void * g, bool h)
         {
           return CVarRegister (name, a, b, c, d, e, f, g, h);
         }
       );

  hook ( add_route_a
       , [] (char const* path, blz::function<void, RenderRequest::RequestData*, route_arg_3_t*> const* fun)
         {
           std::cerr << "add route a: " << path << "\n";
           return add_route_a (path, fun);
         }
       );
  hook ( add_route_b
       , [] (char const* path, blz::function<void, RenderRequest::RequestData*, route_arg_3_t*> const* fun)
         {
           std::cerr << "add route b: " << path << "\n";
           return add_route_b (path, fun);
         }
       );
   
  hook ( renderservice_main
       , []
         {
           add_route ("/test", &callback_test);
           return renderservice_main();
         }
       );
}


extern "C" void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo)
{
  on_inject();
    
  RhWakeUpProcess();
}
 