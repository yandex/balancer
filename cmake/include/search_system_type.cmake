IF (NOT DEFINED __SEARCH_SYSTEM__CMAKE__INCLUDED)

SET (__SEARCH_SYSTEM__CMAKE__INCLUDED true)

IF (MAKE_CHECK)

# disable this palliative for check, as it's too difficult for incredibuild

MACRO (GET_SEARCH_SYSTEM_TYPE var)
    SET(${var} "")
ENDMACRO ()

MACRO (_GET_SEARCH_SYSTEM_TYPE var)
    SET(${var} "")
ENDMACRO ()

FUNCTION (SET_SEARCH_SYSTEM_TYPE type)
ENDFUNCTION ()

FUNCTION (TRY_SET_SEARCH_SYSTEM_TYPE type)
ENDFUNCTION ()

FUNCTION (SET_BUILD_FOR)
ENDFUNCTION ()

FUNCTION (FIX_SEARCH_SYSTEM_TYPE)
ENDFUNCTION ()

FUNCTION (CHK_SEARCH_SYSTEM_TYPE)
ENDFUNCTION ()

FUNCTION (SUPPORTED_SEARCH_SYSTEM_TYPES)
ENDFUNCTION ()

ELSE (MAKE_CHECK)

# search uses different sets of features (matrixnet or otherwise) for different
# types of search systems (web, images etc.) but the codebase is the same; make
# sure we are not building multiple things simultaneously, which would be bad.

SET(SEARCH_SYSTEM_TYPE_DEFAULT "_DEFAULT_")

SET(SEARCH_SYSTEM_TYPE_NOCACHE TRUE)

# 2.6.0 doesn't support IF(x VERSION_LESS y)
IF (SEARCH_SYSTEM_TYPE_NOCACHE OR
    "${CMAKE_MAJOR_VERSION}${CMAKE_MINOR_VERSION}" LESS "28")

MACRO (_GET_SEARCH_SYSTEM_TYPE var)
    get_property(${var} GLOBAL PROPERTY SEARCH_SYSTEM_TYPE)
ENDMACRO ()

MACRO (_GET_SEARCH_SYSTEM_TYPE_ORIGIN var)
    get_property(${var} GLOBAL PROPERTY SEARCH_SYSTEM_TYPE_ORIGIN)
ENDMACRO ()

MACRO (_SET_SEARCH_SYSTEM_TYPE type origin)
    set_property(GLOBAL PROPERTY SEARCH_SYSTEM_TYPE "${type}")
    set_property(GLOBAL PROPERTY SEARCH_SYSTEM_TYPE_ORIGIN "${origin}")
ENDMACRO ()

_SET_SEARCH_SYSTEM_TYPE("" "")

ELSE ()

# clear it from the cache initially so that we can start the process
UNSET(SEARCH_SYSTEM_TYPE CACHE)

MACRO (_GET_SEARCH_SYSTEM_TYPE var)
    get_property(${var} CACHE SEARCH_SYSTEM_TYPE PROPERTY VALUE)
ENDMACRO ()

MACRO (_GET_SEARCH_SYSTEM_TYPE_ORIGIN var)
    get_property(${var} CACHE SEARCH_SYSTEM_TYPE PROPERTY HELPSTRING)
ENDMACRO ()

MACRO (_SET_SEARCH_SYSTEM_TYPE type origin)
    SET(SEARCH_SYSTEM_TYPE "${type}" CACHE STRING "${origin}" FORCE)
ENDMACRO ()

ENDIF ()

# returns type, empty if default, undefined if not set
MACRO (GET_SEARCH_SYSTEM_TYPE var)
    _GET_SEARCH_SYSTEM_TYPE(${var})
    IF ("${${var}}" STREQUAL "")
        SET(${var})
    ELSEIF ("${${var}}" STREQUAL "${SEARCH_SYSTEM_TYPE_DEFAULT}")
        SET(${var} "")
    ENDIF ()
ENDMACRO ()

# takes: type [file [isset_var]]
FUNCTION (SET_SEARCH_SYSTEM_TYPE type)
    IF ("${type}" STREQUAL "")
        SET(type ${SEARCH_SYSTEM_TYPE_DEFAULT})
    ENDIF ()

    _GET_SEARCH_SYSTEM_TYPE(__curtype_)
    IF (__curtype_ STREQUAL type)
        RETURN()
    ENDIF ()

    IF ("${__curtype_}" STREQUAL "")
        SET(__file_ "${ARGV1}")
        IF ("${__file_}" STREQUAL "")
            SET(__file_ "${CMAKE_CURRENT_LIST_FILE}")
        ENDIF ()

        SET(__top_ "${CMAKE_SOURCE_DIR}/")
        STRING(REPLACE ${__top_} "" __file_ ${__file_})
        IF (NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "")
            STRING(REPLACE ${__top_} "" __dir_ ${CMAKE_CURRENT_SOURCE_DIR})
            SET(__dir_ "dir=${__dir_} ")
        ENDIF ()

        _SET_SEARCH_SYSTEM_TYPE("${type}" "${__dir_}file=${__file_}")
        # setting the default type means no preprocessor defines
        IF (NOT type STREQUAL SEARCH_SYSTEM_TYPE_DEFAULT)
            ADD_DEFINITIONS(-DBUILD_FOR_${type} -DBUILD_FOR=${type})
        ENDIF ()
        # in case caller wants to know if we did actually set it
        IF ("${ARGC}" GREATER "2")
            SET(${ARGV2} TRUE PARENT_SCOPE)
        ENDIF ()
        RETURN()
    ENDIF ()

    _GET_SEARCH_SYSTEM_TYPE_ORIGIN(__doc_)
    IF (type STREQUAL SEARCH_SYSTEM_TYPE_DEFAULT)
        SET(what "No preprocessor defines found for search system:")
    ELSE ()
        SET(what "Cannot build multiple search system types: '${type}'; was")
    ENDIF ()
    MESSAGE(SEND_ERROR "${what} '${__curtype_}' [${__doc_}]")
ENDFUNCTION ()

# sets if unset
FUNCTION (TRY_SET_SEARCH_SYSTEM_TYPE type)
    GET_SEARCH_SYSTEM_TYPE(__curtype_)
    IF (NOT DEFINED __curtype_)
        SET_SEARCH_SYSTEM_TYPE("${type}")
    ENDIF ()
ENDFUNCTION ()

FUNCTION (SET_BUILD_FOR)
    SET(__type_ ${ARGV0})
    SET_SEARCH_SYSTEM_TYPE("${__type_}")
    SET(BUILD_FOR_TYPE "${__type_}" PARENT_SCOPE)
    IF (NOT "${__type_}" STREQUAL "")
        SET(BUILD_FOR_${__type_} true PARENT_SCOPE)
    ENDIF ()
ENDFUNCTION ()

# causes the type to be fixed even if not explicitly defined
FUNCTION (FIX_SEARCH_SYSTEM_TYPE)
    set_property(DIRECTORY
        PROPERTY FIX_SEARCH_SYSTEM_TYPE "${CMAKE_CURRENT_LIST_FILE}")
ENDFUNCTION ()

FUNCTION (CHK_SEARCH_SYSTEM_TYPE)
    get_property(__defs_ DIRECTORY PROPERTY COMPILE_DEFINITIONS)
    IF (NOT "${__defs_}" STREQUAL "")
        LIST(REMOVE_DUPLICATES __defs_)
        FOREACH (__item_ ${__defs_})
            IF (__item_ MATCHES "^BUILD_FOR_([^=]+)")
                SET_APPEND(type ${CMAKE_MATCH_1})
            ELSEIF (__item_ MATCHES "^BUILD_FOR=(.+)")
                SET_APPEND(type ${CMAKE_MATCH_1})
            ENDIF ()
        ENDFOREACH ()
    ENDIF ()

    IF ("${type}" STREQUAL "")
        SET(__force_ FALSE)
        IF (NOT "${ARGV0}" STREQUAL "")
            CMAKE_POLICY(PUSH)
            IF (POLICY CMP0012)
                CMAKE_POLICY(SET CMP0012 NEW)
            ENDIF ()
            IF (${ARGV0})
                SET(__force_ TRUE)
            ENDIF ()
            CMAKE_POLICY(POP)
        ENDIF ()
        IF (NOT __force_)
            get_property(__file_ DIRECTORY PROPERTY FIX_SEARCH_SYSTEM_TYPE)
            IF ("${__file_}" STREQUAL "")
                RETURN()
            ENDIF ()
        ENDIF ()
    ELSE ()
        LIST(REMOVE_DUPLICATES type)
        LIST(LENGTH type __ntypes_)
        IF ("${__ntypes_}" GREATER "1")
            MESSAGE(SEND_ERROR
                "Cannot build multiple search system types: ${type}")
            RETURN()
        ENDIF ()
    ENDIF ()

    SET_SEARCH_SYSTEM_TYPE("${type}" ${__file_}) # quote in case it is empty
ENDFUNCTION ()

FUNCTION (SUPPORTED_SEARCH_SYSTEM_TYPES)
    GET_SEARCH_SYSTEM_TYPE(__curtype_)
    IF (NOT DEFINED __curtype_)
        RETURN() # complain, perhaps?
    ENDIF ()
    FOREACH (__type_ ${ARGN})
        IF (__type_ STREQUAL __curtype_)
            RETURN()
        ENDIF ()
    ENDFOREACH ()
    MESSAGE(SEND_ERROR "Current search system type [${__curtype_}] is not supported by this project")
ENDFUNCTION ()

ENDIF () # make check

ENDIF () # file included
