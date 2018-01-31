#include <Windows.h>

BOOL APIENTRY DllMain (HMODULE, DWORD, LPVOID) { return TRUE; }

#include <easyhook.h>
#include <string>
#include <iostream>
#include <Psapi.h>
#include <thread>
#include <chrono>
#include <unordered_map>

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

extern "C" void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo)
{
  hook ( CVarRegister
       , static_cast<CVar* (*)(char const*, char const*, unsigned int, char const*, bool (*)(CVar*, char const*, char const*, void *), unsigned int, bool, void *, bool)>
           ( [] (char const* name, char const* a, unsigned int b, char const* c, bool (*d)(CVar*, char const*, char const*, void *), unsigned int e, bool f, void * g, bool h)
             {
               return CVarRegister (name, a, b, c, d, e, f, g, h);
             }
           )
       );
       
  hook ( add_route_a
       , static_cast<void (*)(char const* path, blz::function<void, RenderRequest::RequestData*, route_arg_3_t*> const* fun)> 
           ( [] (char const* path, blz::function<void, RenderRequest::RequestData*, route_arg_3_t*> const* fun)
             {
               std::cerr << "add route a: " << path << "\n";
               return add_route_a (path, fun);
             }
           )
       );
  hook ( add_route_b
       , static_cast<void (*)(char const* path, blz::function<void, RenderRequest::RequestData*, route_arg_3_t*> const* fun)> 
           ( [] (char const* path, blz::function<void, RenderRequest::RequestData*, route_arg_3_t*> const* fun)
             {
               std::cerr << "add route b: " << path << "\n";
               return add_route_b (path, fun);
             }
           )
       );
	   
  add_route ("/test", &callback_test);
}
