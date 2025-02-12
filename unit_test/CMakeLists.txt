add_executable(unit_test test.cpp ${INCS})

if(APPLE OR CMAKE_SYSTEM_NAME MATCHES "Linux")
  find_library(GTEST_LIBRARY
        NAMES gtest
        PATHS /usr/lib /usr/local/lib
  )
  if(GTEST_LIBRARY)
    target_link_libraries(unit_test LINK_PUBLIC ${GTEST_LIBRARY})
  endif()
endif()



