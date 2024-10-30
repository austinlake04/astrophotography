# FindCCfits.cmake

find_path(CCFITS_INCLUDE_DIR
  NAMES CCfits/CCfits.h
  HINTS ${CCFITS_ROOT_DIR}
        ${BASE_DIR}
  PATH_SUFFIXES include
)

find_library(CCFITS_LIBRARY
  NAMES CCfits
  HINTS ${CCFITS_ROOT_DIR}
        ${BASE_DIR}
  PATH_SUFFIXES lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CCfits
  REQUIRED_VARS CCFITS_LIBRARY CCFITS_INCLUDE_DIR
)

if(CCFITS_FOUND)
  set(CCFITS_LIBRARIES ${CCFITS_LIBRARY})
  set(CCFITS_INCLUDE_DIRS ${CCFITS_INCLUDE_DIR})
endif()

mark_as_advanced(CCFITS_INCLUDE_DIR CCFITS_LIBRARY)
