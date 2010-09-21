PROJECT(maca-convert)

include_directories( ${CMAKE_SOURCE_DIR} )

add_executable( maca-convert main.cpp )

target_link_libraries ( maca-convert maca ${Boost_LIBRARIES} ${LIBS})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

if(UNIX)
	install(TARGETS maca-convert
		RUNTIME DESTINATION bin)
endif(UNIX)
