PROJECT( test )

include_directories( ${CMAKE_SOURCE_DIR}/src/ )

add_definitions(-DLIBTOKI_TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/")

add_executable( tests main.cpp )

target_link_libraries ( tests pltagger ${Boost_LIBRARIES})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

add_custom_target(test tests)
