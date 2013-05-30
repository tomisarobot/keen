
find_package(UnitTest++ REQUIRED)
file(GLOB TEST_HEADER "*.cpp")
file(GLOB TEST_SOURCE "*.h")
include_directories(${UNITTEST++_INCLUDE_DIR})
add_executable(tests ${TEST_SOURCE} ${TEST_HEADER})
target_link_libraries(tests ${UNITTEST++_LIBRARY})
target_link_libraries(tests ${Boost_LIBRARIES})

