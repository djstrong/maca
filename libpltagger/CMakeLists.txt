########## libpltagger ###############

PROJECT(pltagger)

#include_directories("/usr/include/sfst-1.0")
#set(LIBS ${LIBS} "sfst1")

find_package(Toki REQUIRED)
set(LIBS ${LIBS} ${Toki_LIBRARY})

find_package(ICU REQUIRED QUIET)
include_directories(${ICU_INCLUDE_DIR})
link_directories(${ICU_LIBRARY_DIRS})
set(LIBS ${LIBS} icuuc icuio)

find_package(Loki REQUIRED QUIET)
set(LIBS ${LIBS} loki)

find_package(LibXML++ REQUIRED QUIET)
include_directories(${LibXML++_INCLUDE_DIRS})
link_directories(${LibXML++_LIBRARY_DIRS})
set(LIBS ${LIBS} ${LibXML++_LIBRARIES})

link_directories(${Boost_LIBRARY_DIRS})
set(LIBS ${LIBS} ${Boost_LIBRARIES})

SET(libpltagger_STAT_SRC
    confignode.cpp
    conv/attributecopier.cpp
    conv/fold.cpp
    conv/joinlayer.cpp
    conv/joinrule.cpp
    conv/layer.cpp
    conv/predicate.cpp
    conv/splitlayer.cpp
    conv/tagconvert.cpp
    conv/tagrule.cpp
    conv/tagrulelayer.cpp
    conv/tagsetconverter.cpp
    debug.cpp
    exception.cpp
    lexeme.cpp
    main.cpp
    morph/constanalyser.cpp
    morph/creator.cpp
    morph/dispatchanalyser.cpp
    morph/mapanalyser.cpp
    morph/morphanalyser.cpp
    settings.cpp
    symboldictionary.cpp
    tag.cpp
    tagset.cpp
    tagsetmanager.cpp
    tagsetparser.cpp
    token.cpp
    tokensource.cpp
)

if (SFST_FOUND)
    SET(libpltagger_STAT_SRC ${libpltagger_STAT_SRC} morph/sfstanalyser.cpp)
endif (SFST_FOUND)

if (MORFEUSZ_FOUND)
    SET(libpltagger_STAT_SRC ${libpltagger_STAT_SRC}
    morph/morfeuszanalyser.cpp
    morph/morfeuszcompat.cpp
    )
endif (MORFEUSZ_FOUND)

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
        install(
                DIRECTORY ${LIBPLTAGGER_SRC_DATA_DIR}/
                DESTINATION ${LIBPLTAGGER_INSTALL_DATA_DIR}
                FILES_MATCHING PATTERN "*.ini"
                 PATTERN "*.tagset"
                 PATTERN "*.conv"
                 PATTERN "*.ini"
                 PATTERN "*.xml"
                 PATTERN "*.txt"
                PATTERN ".svn" EXCLUDE
        )
endif(UNIX)

