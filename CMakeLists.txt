PROJECT(MorphAnalyser)
cmake_minimum_required(VERSION 2.8.0)

set(maca_ver_major "1")
set(maca_ver_minor "0")
set(maca_ver_patch "4")

set(LIBMACA_VERSION "${maca_ver_major}.${maca_ver_minor}.${maca_ver_patch}")

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

		set(CMAKE_C_FLAGS "-W -Wall -ansi $ENV{CFLAGS}" 
			CACHE STRING "Flags used by the C compiler during normal builds."
			FORCE)
		set(CMAKE_C_FLAGS_DEBUG "-O0 -DDEBUG -ggdb3 -W -Wall -ansi $ENV{CFLAGS}"
			CACHE STRING "Flags used by the C compiler during debug builds."
			FORCE)

		set(CMAKE_CXX_FLAGS "-W -Wall -ansi ${STRICT_FLAGS} $ENV{CXXFLAGS}"
			CACHE STRING "Flags used by the CXX compiler during normal builds."
			FORCE)
		set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DDEBUG -ggdb3 -W -Wall -ansi ${STRICT_FLAGS} $ENV{CXXFLAGS}"
			CACHE STRING "Flags used by the CXX compiler during debug builds."
			FORCE)
                set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -fno-omit-frame-pointer" 
                        CACHE STRING "Flags used by the CXX compiler during relwithdebiginfo builds" FORCE)
	endif(NOT CONFIGURED)
endif(CMAKE_COMPILER_IS_GNUCXX)

set(LIBS "")
include_directories(${MorphAnalyser_SOURCE_DIR})

FIND_PATH(LIBMACA_SRC_DATA_DIR
	test.ini
	${CMAKE_SOURCE_DIR}/data
)
MARK_AS_ADVANCED(LIBMACA_SRC_DATA_DIR)

find_package(Boost 1.41 REQUIRED COMPONENTS program_options filesystem regex)

add_subdirectory(libmaca)
add_subdirectory(maca-analyse)
add_subdirectory(maca-reanalyse)
add_subdirectory(maca-app)
add_subdirectory(maca-convert)
add_subdirectory(tests)

if (UNIX)
	install(FILES tools/bash_completion_maca DESTINATION /etc/bash_completion.d)
endif(UNIX)
