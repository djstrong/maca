PROJECT(maca-app)

include_directories( ${CMAKE_SOURCE_DIR} )

add_executable( maca-app main.cpp )

target_link_libraries ( maca-app maca ${Boost_LIBRARIES} ${LIBS})

include_directories(${maca_BINARY_DIR}/include})
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

if(UNIX)
	install(TARGETS maca-app
		RUNTIME DESTINATION bin)
endif(UNIX)
