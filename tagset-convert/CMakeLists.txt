PROJECT(tagset-convert)

include_directories( ${CMAKE_SOURCE_DIR} )

add_executable( tagset-convert main.cpp )

target_link_libraries ( tagset-convert pltagger ${Boost_LIBRARIES} ${LIBS})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

if(UNIX)

        install(TARGETS tagset-convert
                RUNTIME DESTINATION bin)
        
endif(UNIX)
