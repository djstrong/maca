PROJECT(Tagger)
cmake_minimum_required(VERSION 2.6.0)

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

configure_file(src/config_d.in include/config_d.h)
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/include/ )
add_definitions(-DHAVE_CONFIG_D_H)

find_package(Boost 1.41 REQUIRED COMPONENTS program_options filesystem regex)

add_subdirectory(src)
add_subdirectory(src/app)
add_subdirectory(tests)
