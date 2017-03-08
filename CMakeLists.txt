cmake_minimum_required (VERSION 2.8) 

project (CumBuffer)

file(GLOB_RECURSE INCS "*.h")
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)    
set(CMAKE_CXX_FLAGS_DEBUG          " /MTd ")
set(CMAKE_CXX_FLAGS_RELEASE        " /MT  ")
set(CMAKE_CXX_FLAGS_MINSIZEREL     " /MT  ")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO " /MT  ")
endif()

set(CMAKE_CXX_STANDARD 11) 

if(WIN32)    
add_definitions(/W4)
add_definitions(/wd4244)
elseif(APPLE OR CMAKE_SYSTEM_NAME MATCHES "Linux")    
add_definitions(-Wall)
endif()

add_subdirectory (unit_test)


