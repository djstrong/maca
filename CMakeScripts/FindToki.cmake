FIND_PATH(Toki_INCLUDE_DIR token.h /usr/include/libtoki /usr/local/include/libtoki )

FIND_LIBRARY(Toki_LIBRARY NAMES toki PATH /usr/lib /usr/local/lib) 

MARK_AS_ADVANCED(Toki_LIBRARY)
MARK_AS_ADVANCED(Toki_INCLUDE_DIR)


IF (Toki_INCLUDE_DIR AND Toki_LIBRARY)
   SET(Toki_FOUND TRUE)
ENDIF (Toki_INCLUDE_DIR AND Toki_LIBRARY)


IF (Toki_FOUND)
   IF (NOT Toki_FIND_QUIETLY)
      MESSAGE(STATUS "Found libtoki: ${Toki_LIBRARY}")
   ENDIF (NOT Toki_FIND_QUIETLY)
ELSE (Toki_FOUND)
   IF (Toki_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find libtoki")
   ELSE (Toki_FIND_REQUIRED)
      MESSAGE(STATUS "libtoki not found")
   ENDIF (Toki_FIND_REQUIRED)
ENDIF (Toki_FOUND)
