

include(FindPkgConfig)
include(FindPackageHandleStandardArgs)

pkg_check_modules(GLIB_PKGCONF REQUIRED glibmm-2.4)

find_path(GLIB_INCLUDE_DIR
        NAMES glibmm
        PATHS ${GLIB_PKGCONF_INCLUDE_DIRS})

find_library(GLIB_LIBRARY
        NAMES glib
        PATHS ${GLIB_PKGCONF_LIBRARY_DIRS})

if(GLIB_PKGCONF_FOUND)
    set(GLIB_LIBRARIES ${GLIB_LIBRARY} ${GLIB_LIBRARIES})
    set(GLIB_INCLUDE_DIRS ${GLIB_INCLUDE_DIR} ${GLIB_INCLUDE_DIRS})
    set(GLIB_FOUND yes)
else()
    set(GLIB_LIBRARIES)
    set(GLIB_INCLUDE_DIRS)
    set(GLIB_FOUND no)
endif()
