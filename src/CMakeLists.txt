########## libtokenizer ###############

PROJECT(pltagger)

set(ver_major "0")
set(ver_minor "1")

set(LIBS "")

find_package(Toki REQUIRED)
set(LIBS ${LIBS} ${Toki_LIBRARY})

find_package(ICU REQUIRED)
include_directories(${ICU_INCLUDE_DIR})
link_directories(${ICU_LIBRARY_DIRS})
set(LIBS ${LIBS} icuuc icuio)

find_package(Loki REQUIRED)
set(LIBS ${LIBS} loki)

find_package(LibXML++ REQUIRED)
include_directories(${LibXML++_INCLUDE_DIRS})
link_directories(${LibXML++_LIBRARY_DIRS})
set(LIBS ${LIBS} ${LibXML++_LIBRARIES})

link_directories(${Boost_LIBRARY_DIRS})
set(LIBS ${LIBS} ${Boost_LIBRARIES})

include_directories( ${CMAKE_SOURCE_DIR}/src/ )

set(LIBPLTAGGER_VERSION "${ver_major}.${ver_minor}")


configure_file(config_d.in include/config_d.h)
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/include/ )
add_definitions(-DHAVE_CONFIG_D_H)

SET(libpltagger_STAT_SRC
    debug.cpp
    lexeme.cpp
    main.cpp
    token.cpp
)

add_library(pltagger SHARED ${libpltagger_STAT_SRC})
target_link_libraries ( pltagger ${LIBS} )
set_target_properties(pltagger PROPERTIES
        VERSION "${ver_major}.${ver_minor}"
        SOVERSION ${ver_major})

if(UNIX)
    set_target_properties(pltagger PROPERTIES
        LINK_FLAGS -Wl,-e,LIBPLTAGGER_entry_)
endif(UNIX)


if(UNIX)
        install(TARGETS pltagger
                LIBRARY DESTINATION lib
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

        install(
                DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                DESTINATION include
                FILES_MATCHING PATTERN "*.h"
                PATTERN ".svn" EXCLUDE
                PATTERN "bin" EXCLUDE
                PATTERN "build" EXCLUDE 
                PATTERN "CMake*" EXCLUDE
        )
endif(UNIX)

