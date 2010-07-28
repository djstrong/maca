PROJECT(tagset-tool)

include_directories( ${CMAKE_SOURCE_DIR} )

add_executable( tagset-tool main.cpp )

target_link_libraries ( tagset-tool pltagger ${Boost_LIBRARIES} ${LIBS})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

if(UNIX)

        install(TARGETS tagset-tool
                RUNTIME DESTINATION bin)
        
endif(UNIX)
