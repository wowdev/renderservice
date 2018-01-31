cmake_minimum_required(VERSION 3.2)

if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release)
endif()

set (CMAKE_LIBRARY_ARCHITECTURE "x64/v140/${CMAKE_BUILD_TYPE}")

set (easyhook_version "2.7.6578.1")
set (CMAKE_PREFIX_PATH "${CMAKE_CURRENT_BINARY_DIR}/EasyHookNativePackage.redist.${easyhook_version}/build/native"
                       "${CMAKE_CURRENT_BINARY_DIR}/EasyHookNativePackage.${easyhook_version}/build/native"
)

message (STATUS "${CMAKE_INSTALL_PREFIX}")

find_library (easyhook_lib NAMES EasyHook64.lib)
find_file (easyhook_redist NAMES EasyHook64.dll PATH_SUFFIXES "bin/${CMAKE_LIBRARY_ARCHITECTURE}")
find_path (easyhook_include NAMES easyhook.h)

if(NOT easyhook_lib)
  message (FATAL_ERROR "${easyhook_lib}")
endif()
if(NOT easyhook_redist)
  message (FATAL_ERROR "${easyhook_redist}")
endif()
if(NOT easyhook_include)
  message (FATAL_ERROR "${easyhook_include}")
endif()

add_executable (RenderServiceInjector "injector.cpp")
add_library (RenderServiceHook SHARED "hook.cpp")

target_include_directories (RenderServiceInjector SYSTEM PRIVATE "${easyhook_include}")
target_include_directories (RenderServiceHook SYSTEM PRIVATE "${easyhook_include}")

target_link_libraries (RenderServiceInjector ${easyhook_lib})
target_link_libraries (RenderServiceHook Psapi ${easyhook_lib})

install (TARGETS RenderServiceInjector RenderServiceHook DESTINATION ".")
install (FILES "${easyhook_redist}" DESTINATION ".")