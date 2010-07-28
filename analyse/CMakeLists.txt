PROJECT(analyse)

include_directories( ${CMAKE_SOURCE_DIR} )

add_executable( analyse main.cpp )

target_link_libraries ( analyse pltagger ${Boost_LIBRARIES} ${LIBS})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

if(UNIX)

        install(TARGETS analyse
                RUNTIME DESTINATION bin)
        
endif(UNIX)
