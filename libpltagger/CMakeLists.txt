########## libpltagger ###############

PROJECT(pltagger)

set(LIBPLTAGGER_PATH_SEPARATOR ";")
set(LIBPLTAGGER_DATA_DIR ".;${CMAKE_INSTALL_PREFIX}/${LIBPLTAGGER_INSTALL_DATA_DIR}")
configure_file(config_d.in include/libpltagger/config_d.h)
add_definitions(-DHAVE_CONFIG_D_H)

include_directories( ${CMAKE_CURRENT_BINARY_DIR}/include/ )


#include_directories("/usr/include/sfst-1.0")
#set(LIBS ${LIBS} "sfst1")

find_package(Toki 0.3.2 REQUIRED)
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
    chunk.cpp
    exception.cpp
    iob.cpp
    lexeme.cpp
    main.cpp
    sentence.cpp
    tag.cpp
    tagset.cpp
    tagsetmanager.cpp
    tagsetparser.cpp
    token.cpp
    tokensource.cpp
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
    io/init.cpp
    io/orthwriter.cpp
    io/plain.cpp
    io/premorph.cpp
    io/reader.cpp
    io/writer.cpp
    io/xces.cpp
    io/xcescommon.cpp
    io/xcesreader.cpp
    io/xcesvalidate.cpp
    io/xceswriter.cpp
    morph/constanalyser.cpp
    morph/dispatchanalyser.cpp
    morph/mapanalyser.cpp
    morph/morphanalyser.cpp
    util/confignode.cpp
    util/debug.cpp
    util/settings.cpp
    util/symboldictionary.cpp
)

if (UNIX)
    set(MORFEUSZ_PLUGIN ON CACHE BOOL "Build Morfeusz analyser as a plugin")
    set(SFST_PLUGIN ON CACHE BOOL "Build SFST analyser as a plugin")
endif(UNIX)

find_package(SFST)
if (SFST_FOUND)
    if(SFST_PLUGIN)
        message(STATUS "SFST support will be built as a plugin")
        add_library(pltagger_sfst MODULE morph/sfstanalyser.cpp)
        target_link_libraries ( pltagger_sfst pltagger ${SFST_LIBRARY} )
        set_target_properties(pltagger_sfst PROPERTIES
            VERSION "${ver_major}.${ver_minor}"
            SOVERSION ${ver_major})
        set_target_properties(pltagger_sfst PROPERTIES
            COMPILE_FLAGS "-DSFST_AS_PLUGIN -I${SFST_INCLUDE_DIR}"
        )
        install(TARGETS pltagger_sfst
                LIBRARY DESTINATION lib)
    else(SFST_PLUGIN)
        set(LIBS ${LIBS} ${SFST_LIBRARY})
        include_directories(${SFST_INCLUDE_DIR})
        link_directories(${SFST_LIBRARY_DIRS})
        set(HAVE_SFST 1)
        SET(libpltagger_STAT_SRC ${libpltagger_STAT_SRC} morph/sfstanalyser.cpp)
    endif(SFST_PLUGIN)
endif (SFST_FOUND)

find_package(Morfeusz)
if (MORFEUSZ_FOUND)
    if(MORFEUSZ_PLUGIN)
        message(STATUS "Morfeusz support will be built as a plugin")
        add_library(pltagger_morfeusz MODULE morph/morfeuszanalyser.cpp)
        target_link_libraries ( pltagger_morfeusz pltagger ${MORFEUSZ_LIBRARY} )
        set_target_properties(pltagger_morfeusz PROPERTIES
            VERSION "${ver_major}.${ver_minor}"
            SOVERSION ${ver_major})
        set_target_properties(pltagger_morfeusz PROPERTIES
            COMPILE_FLAGS "-DMORFEUSZ_AS_PLUGIN -I${MORFEUSZ_INCLUDE_DIR}"
        )
        install(TARGETS pltagger_morfeusz
                LIBRARY DESTINATION lib)
    else(MORFEUSZ_PLUGIN)
        set(LIBS ${LIBS} ${MORFEUSZ_LIBRARY})
        include_directories(${MORFEUSZ_INCLUDE_DIR})
        link_directories(${MORFEUSZ_LIBRARY_DIRS})
        set(HAVE_MORFEUSZ 1)
        SET(libpltagger_STAT_SRC ${libpltagger_STAT_SRC} morph/morfeuszanalyser.cpp)
    endif(MORFEUSZ_PLUGIN)
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
                 PATTERN "*.xml"
                 PATTERN "*.txt"
				 PATTERN "*.fst"
                PATTERN ".svn" EXCLUDE
                PATTERN "local/*" EXCLUDE
        )
endif(UNIX)

