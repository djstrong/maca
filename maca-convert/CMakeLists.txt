PROJECT(maca-convert)

include_directories( ${CMAKE_SOURCE_DIR} )

add_executable( maca-convert main.cpp )

find_package(Toki 1.0.0 REQUIRED)
set(LIBS ${LIBS} ${Toki_LIBRARY})

find_package(Corpus2 1.0.9 REQUIRED)
set(LIBS ${LIBS} ${Corpus2_LIBRARIES})

target_link_libraries ( maca-convert maca ${Boost_LIBRARIES} ${LIBS})

include_directories(${maca_BINARY_DIR}/include)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

if(UNIX)
	install(TARGETS maca-convert
		RUNTIME DESTINATION bin)
endif(UNIX)
