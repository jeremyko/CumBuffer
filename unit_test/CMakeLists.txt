add_executable (unit_test test.cpp ${INCS})    

set(CMAKE_CXX_STANDARD 11) 

if(APPLE OR CMAKE_SYSTEM_NAME MATCHES "Linux")    
    find_library (GTEST_LIBRARY
        NAMES gtest
        PATHS /usr/lib /usr/local/lib
    )
    if (GTEST_LIBRARY)
        #target_link_libraries (unit_test LINK_PUBLIC libgtest.a)
        target_link_libraries (unit_test LINK_PUBLIC ${GTEST_LIBRARY})
    endif ()
endif()

if( CMAKE_SYSTEM_NAME MATCHES "Linux")    
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")
endif ()


