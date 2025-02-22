cmake_minimum_required(VERSION 3.14)

project(CumBuffer)

file(GLOB_RECURSE INCS "*.h")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)

if(APPLE)
  include_directories(/usr/local/include)
endif()

if(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /nologo /Gy /EHsc")
  set(CMAKE_CXX_FLAGS_DEBUG          " /MTd ")
  set(CMAKE_CXX_FLAGS_RELEASE        " /MT /O2 ")
  set(CMAKE_CXX_FLAGS_MINSIZEREL     " /MT /O2 ")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO " /MT /O2 ")
endif()

set(CMAKE_CXX_STANDARD 14)
if(APPLE)
  add_definitions(-std=c++14)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE true)

if(WIN32)
  add_definitions(/W4)
  add_definitions(/wd4244)
elseif(APPLE OR CMAKE_SYSTEM_NAME MATCHES "Linux")
  add_definitions(-Wall)
  add_definitions(-Werror)
  add_definitions(-Wextra)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

add_subdirectory(unit_test)

