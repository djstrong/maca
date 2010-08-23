PROJECT(Tagger)
cmake_minimum_required(VERSION 2.8.0)

set(ver_major "0")
set(ver_minor "1")
set(ver_patch "1")

# use some of our own Find* scripts
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeScripts)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release
RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)


if(CMAKE_COMPILER_IS_GNUCXX)
	# Set our own default flags at first run.
	if(NOT CONFIGURED)

		if(ENABLE_STRICT_COMPILATION)
			set(STRICT_FLAGS "-Werror -Wno-unused-parameter")
		else(ENABLE_STRICT_COMPILATION)
			set(STRICT_FLAGS "")
		endif(ENABLE_STRICT_COMPILATION)


		# Strict compilation for C files is disabled until somebody wants to clean them.
		set(CMAKE_C_FLAGS"-W -Wall -ansi $ENV{CFLAGS}" 
				CACHE STRING "Flags used by the C compiler during normal builds." FORCE)
		set(CMAKE_C_FLAGS_DEBUG "-O0 -DDEBUG -ggdb3 -W -Wall -ansi $ENV{CFLAGS}"
				CACHE STRING "Flags used by the C compiler during debug builds." FORCE)

		set(CMAKE_CXX_FLAGS "-W -Wall -ansi ${STRICT_FLAGS} $ENV{CXXFLAGS}"
				CACHE STRING "Flags used by the CXX compiler during normal builds." FORCE)
		set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DDEBUG -ggdb3 -W -Wall -ansi ${STRICT_FLAGS} $ENV{CXXFLAGS}"
				CACHE STRING "Flags used by the CXX compiler during debug builds." FORCE)

	endif(NOT CONFIGURED)

endif(CMAKE_COMPILER_IS_GNUCXX)

set(LIBS "")
include_directories( ${Tagger_SOURCE_DIR} )

find_package(SFST)
if (SFST_FOUND)
	set(LIBS ${LIBS} ${SFST_LIBRARY})
	include_directories(${SFST_INCLUDE_DIR})
	link_directories(${SFST_LIBRARY_DIRS})
	set(HAVE_SFST 1)
endif(SFST_FOUND)

find_package(Morfeusz)
if (MORFEUSZ_FOUND)
	set(LIBS ${LIBS} ${MORFEUSZ_LIBRARY})
	include_directories(${MORFEUSZ_INCLUDE_DIR})
	link_directories(${MORFEUSZ_LIBRARY_DIRS})
	set(HAVE_MORFEUSZ 1)
endif(MORFEUSZ_FOUND)

set(LIBPLTAGGER_INSTALL_DATA_DIR lib/pltagger)
set(LIBPLTAGGER_VERSION "${ver_major}.${ver_minor}.${ver_patch}")
FIND_PATH(LIBPLTAGGER_SRC_DATA_DIR
    test.tagset
    ${CMAKE_SOURCE_DIR}/data
)
MARK_AS_ADVANCED(LIBPLTAGGER_SRC_DATA_DIR)

find_package(Boost 1.41 REQUIRED COMPONENTS program_options filesystem regex)

add_subdirectory(libpltagger)
add_subdirectory(analyse)
add_subdirectory(app)
add_subdirectory(tagset-convert)
add_subdirectory(tagset-tool)
add_subdirectory(tests)
