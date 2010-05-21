PROJECT(tagger-app)

include_directories( ${CMAKE_SOURCE_DIR}/src/ )

add_executable( tagger-app main.cpp )

target_link_libraries ( tagger-app pltagger ${Boost_LIBRARIES})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

if(UNIX)

        install(TARGETS tagger-app
                RUNTIME DESTINATION bin)
        
endif(UNIX)
