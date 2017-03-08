add_executable (unit_test test.cpp ${INCS})    

if(APPLE OR CMAKE_SYSTEM_NAME MATCHES "Linux")    
target_link_libraries (unit_test LINK_PUBLIC libgtest.a)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")
endif()

set(CMAKE_CXX_STANDARD 11) 

