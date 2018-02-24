#pragma once

#include "blz_stl.hpp"
#include "common_types.hpp"

struct _UNKNOWN;

struct struct_4
{
  C3Vector position;
  C3Vector target;
  float nearClip;
  float farClip;
  float fovyInRadians;
  float aspect;
  C3Vector field_30;
};

struct struc_10;
struct struc_11
{
  struc_10 *field_0;
  struct_4 world_update_params;
  C44Matrix field_3C;
  C44Matrix field_7C;
  C3Vector field_BC;
  char gapC8[160];
  _UNKNOWN *ffxEffectDeath;
  _UNKNOWN *ffxEffectGlow;
  _UNKNOWN *ffxEffectNetherWorld;
  _UNKNOWN *ffxEffectSwirlingFog;
  _UNKNOWN *ffxEffectUnderwater;
  _UNKNOWN *ffxEffectUnconscious;
  _UNKNOWN *ffxEffectPenAndInk;
  _UNKNOWN *ffxEffectColorGrading;
  _UNKNOWN *ffxEffectSpectralSight;
  int screenEffect;
  int currentScreenEffects[2];
  int field_1BC;
  int field_1C0;
  int field_1C4;
};

struct http_message;
struct struc_12;
struct RenderRequest
{
	char _1[0x20];
	struct RequestData
	{
    blz::string resource;
		blz::string path_token;
    
    char _1[0xa0-0x20];
    
    //static fun<char const* (RequestData::*) (char const* name, char const* default_value)> get_param;
	} requestData;
  
  http_message* http_context;
  int http_status;
  int padding_0xCC;

	struct Context
	{
		void* field_0;
		struc_11* field_8;
	} context;
  
  struc_12* field_E0;

	//static fun<bool (RenderRequest::*)()> sub_140053310;
  //static fun<bool (RenderRequest::*)()> AcquireScene;
  //static fun<void (RenderRequest::*)()> Render;
};

struct http_message
{
  struct buffer
  {
    static fun<void* (buffer::*) (void const*, size_t)> append;
  };
  void *header;
  void *content;
  int http_status;
  int field_14;
  const char *http_status_name;
  const char *headers[16];
  char field_A0;
  int field_A4;
  int field_A8;
  int field_AC;
  static fun<void (http_message::*) (int)> set_status;
  static fun<void (http_message::*) (int, char const*)> set_header;
  static fun<void (http_message::*)()> send_and_release;
  static fun<buffer* (http_message::*)()> alloc_content;
};
decltype (http_message::set_status) http_message::set_status 
  = search_pattern ("40 53 48 83 EC 20 4C 8D 0D ?? ?? ?? ?? 48 8B D9 48 8B 0D ?? ?? ?? ?? 49 8B C1 45 33 C0 0F 1F 00");
decltype (http_message::set_header) http_message::set_header = -1;
decltype (http_message::send_and_release) http_message::send_and_release 
  = search_pattern ("40 53 48 83 EC 40 48 8B D9 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 4C 8B 05 ?? ?? ?? ?? 4D 85 C0 0F 84 E3 01 00 00");
decltype (http_message::alloc_content) http_message::alloc_content = -1;
decltype (http_message::buffer::append) http_message::buffer::append = -1;

struct route_arg_3_t
{
  http_message* message;
  void message_set_status (int status) const { http_message::set_status (message, status); }
  void message_send_and_release() const { http_message::send_and_release (message); }
};

struct CVar;
fun<CVar* (char const*, char const*, unsigned int, char const*, bool (*)(CVar*, char const*, char const*, void *), unsigned int, bool, void *, bool)> CVarRegister 
  = search_pattern ("48 8B C4 55 56 41 ?? 41 57 48 83 EC 48 4D 8B F9 41 8B F0 4C 8B ?? 48 8B E9 48 85 C9 0F 84 ?? ?? ?? ?? 80 39 00");
  // WOW-23038patch7.1.5_PTR: 0x140062830
  // WOW-25902patch8.0.1_Beta: 0x1400638D0

//! \note we don't actually want to allow_multiple, but GET and POST are pretty much entirely the same so differing them is hard, so we just call both...
fun<void (char const*, blz::function<void, RenderRequest::RequestData*, route_arg_3_t*> const*)> add_route_a
  = search_pattern ("48 89 5C 24 08 48 89 74 24 10 55 57 41 54 41 56 41 57 48 8B EC 48 81 EC 80 00 00 00 45 33 E4 48 8D 45 B8 45 0F B6 F8 48 89 45", allow_multiple);
fun<void (char const*, blz::function<void, RenderRequest::RequestData*, route_arg_3_t*> const*)> add_route_b
  = search_pattern ("48 89 5C 24 08 48 89 74 24 10 55 57 41 54 41 56 41 57 48 8B EC 48 81 EC 80 00 00 00 45 33 E4 48 8D 45 B8 45 0F B6 F8 48 89 45", allow_multiple, add_route_a._offset + 1);
  // WOW-?????patch7.?.?_PTR: 0x140198550
  // WOW-25902patch8.0.1_Beta: 0x140293FB0
void add_route (char const* path, blz::function<void, RenderRequest::RequestData*, route_arg_3_t*> const* fun)
{
  add_route_a (path, fun);
  add_route_b (path, fun);
}
