PROJECT(maca-reanalyse)

include_directories( ${CMAKE_SOURCE_DIR} )

add_executable( maca-reanalyse main.cpp )

target_link_libraries ( maca-reanalyse maca ${Boost_LIBRARIES} ${LIBS})

include_directories(${maca_BINARY_DIR}/include})
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

find_package(OpenMP)
if (OPENMP_FOUND)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${OpenMP_CXX_FLAGS}")
endif(OPENMP_FOUND)


if(UNIX)
	install(TARGETS maca-reanalyse
		RUNTIME DESTINATION bin)
endif(UNIX)
