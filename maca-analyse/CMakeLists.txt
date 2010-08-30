PROJECT(maca-analyse)

include_directories( ${CMAKE_SOURCE_DIR} )

add_executable( maca-analyse main.cpp )

target_link_libraries ( maca-analyse maca ${Boost_LIBRARIES} ${LIBS})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

if(UNIX)

        install(TARGETS maca-analyse
                RUNTIME DESTINATION bin)
        
endif(UNIX)
