cmake_minimum_required (VERSION 2.8) 

project (CumBuffer)

file(GLOB_RECURSE INCS "*.h")
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

if(APPLE)    
INCLUDE_DIRECTORIES(/usr/local/include)
endif()

if(WIN32)    
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /nologo /Gy /EHsc")
set(CMAKE_CXX_FLAGS_DEBUG          " /MTd ")
set(CMAKE_CXX_FLAGS_RELEASE        " /MT /O2 ")
set(CMAKE_CXX_FLAGS_MINSIZEREL     " /MT /O2 ")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO " /MT /O2 ")
endif()

set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_VERBOSE_MAKEFILE true)


if(WIN32)    
add_definitions(/W4)
add_definitions(/wd4244)
elseif(APPLE OR CMAKE_SYSTEM_NAME MATCHES "Linux")    
add_definitions(-Wall)
endif()

add_subdirectory (unit_test)


