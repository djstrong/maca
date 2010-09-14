PROJECT( test )

include_directories( ${CMAKE_SOURCE_DIR} )

add_definitions(-DLIBTOKI_TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/")

add_executable( tests
	main.cpp
	basic.cpp
	conv.cpp
	morph_basic.cpp
	tag_split.cpp
	tagset_parse.cpp
)

target_link_libraries ( tests maca ${Boost_LIBRARIES})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

configure_file(test-sanity.sh ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(test tests COMMAND ./test-sanity.sh small)
add_custom_target(test-large ./test-sanity.sh large)
