enable_testing()

add_executable(intervaltests
  ${CMAKE_CURRENT_SOURCE_DIR}/IntervalTest.cpp
  )

target_include_directories(intervaltests PRIVATE
  ${CMAKE_SOURCE_DIR}/src/intervalrangepass
)

target_link_libraries(intervaltests PRIVATE 
    Catch2::Catch2WithMain
    interval)


catch_discover_tests(intervaltests)
