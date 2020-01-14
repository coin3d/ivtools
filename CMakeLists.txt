cmake_minimum_required (VERSION 3.9.1)

# Add path for Coin specific utility scripts
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")
include(CoinCMakeUtilities)

coin_project(ivtools VERSION 1.0.0 DESCRIPTION "Inventor Mentor tools for the Coin3D collection.")

# ############################################################################
# CMake module includes
# ############################################################################

include(GNUInstallDirs)
include(CheckCXXSymbolExists)
include(CheckIncludeFileCXX)

# ############################################################################
# Provide options to customise the build
# ############################################################################

function(report_prepare)
  set(multiValueArgs IF_APPLE IF_WIN32)
  cmake_parse_arguments(REPORT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
  if (REPORT_IF_APPLE AND APPLE)
    list(APPEND res ${REPORT_IF_APPLE})
  endif()
  if (REPORT_IF_WIN32 AND WIN32)
    list(APPEND res ${REPORT_IF_WIN32})
  endif()
  list(APPEND res ${REPORT_UNPARSED_ARGUMENTS})
  list(APPEND PACKAGE_OPTIONS ${res})
  set(PACKAGE_OPTIONS "${PACKAGE_OPTIONS}" PARENT_SCOPE)
endfunction(report_prepare)

option(HAVE_ASAN     "Link executable with the Address Sanitizer static library")
option(HAVE_DEBUG    "Add debugging information during the configure process")
option(IVT_USE_CPACK "If enabled the cpack subrepo is mandatory" OFF)

report_prepare(
  HAVE_ASAN
)

if(POLICY CMP0072)
  # get rid of OpenGL GLVND warning from CMake 3.11
  cmake_policy(SET CMP0072 NEW)
endif()

if (HAVE_ASAN)
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
  set (CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -lasan")
endif(HAVE_ASAN)

# ############################################################################
# Target environment checks
# ############################################################################

check_include_file_cxx(unistd.h HAVE_UNISTD_H)
if (HAVE_UNISTD_H)
  add_definitions(-DHAVE_UNISTD_H)
endif()
check_symbol_exists(getopt "unistd.h" HAVE_GETOPT)
if (HAVE_GETOPT)
  add_definitions(-DHAVE_GETOPT)
else()
  add_definitions(-DHAVE_LOCAL_GETOPT_H) # use locally provided getopt
endif()
check_symbol_exists(isatty "unistd.h" HAVE_ISATTY)
if (HAVE_ISATTY)
  add_definitions(-DHAVE_ISATTY)
endif()
if (MSVC)
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_USE_MATH_DEFINES -DNOMINMAX)
endif()

find_package(Coin REQUIRED)
find_package(So${Gui} REQUIRED COMPONENTS merda)

# ############################################################################
# Project setup
# ############################################################################

if(NOT HAVE_GETOPT)
  add_subdirectory(getopt)
  list(APPEND IVTOOLS_ADDITIONAL_LIBRARIES localgetopt)
endif()

add_subdirectory(ivNodeKitStructure)
add_subdirectory(ivcat)
add_subdirectory(ivdefaults)
add_subdirectory(ivfix)
add_subdirectory(ivhierarchy)
add_subdirectory(ivperf)
add_subdirectory(renderos)

############################################################################
# New CPACK section, please see the README file inside cpack.d directory.
if (IVT_USE_CPACK)
  add_subdirectory(cpack.d)
endif()
