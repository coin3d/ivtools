macro(coin_setup_gui_project)
  string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
  string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)

  # ############################################################################
  # GUI target preliminary setup
  set(Gui "Qt" CACHE STRING "Target GUI for the Open Inventor examples")
  set(GuiValues "Qt;Xt;Win" CACHE INTERNAL "List of supported values for the Gui cache variable")
  set_property(CACHE Gui PROPERTY STRINGS ${GuiValues})
  message(STATUS "Gui='${Gui}'")
  if (Gui STREQUAL "Qt")
    set(WINWIDGET QWidget*)
    set(GUIWINDOW QWidget*)
    set(GUIDISPLAY QWidget*)
  elseif(Gui STREQUAL "Xt")
    set(WINWIDGET Widget)
    set(GUIWINDOW Window)
    set(GUIDISPLAY Display*)
  elseif(Gui STREQUAL "Win")
    set(WINWIDGET HWND)
    set(GUIWINDOW HWND)
    set(GUIDISPLAY HWND)
  elseif(Gui STREQUAL "Wx")
    set(WINWIDGET wxWindow*)
    set(GUIWINDOW wxWindow*)
    set(GUIDISPLAY wxWindow*)
  else()
    message(FATAL_ERROR "Only Qt, Xt, Wx and Win are supported: please set Gui to one of these values")
  endif()
  string(TOUPPER ${Gui} GUI)
  # ############################################################################

  string(TIMESTAMP PROJECT_BUILD_YEAR "%Y")
endmacro()

# option controlled helper for cmake variable dumping during config
function(dump_variable)
  if (HAVE_DEBUG)
    foreach(f ${ARGN})
      if (DEFINED ${f})
        message("${f} = ${${f}}")
      else()
        message("${f} = ***UNDEF***")
      endif()
    endforeach()
  endif()
endfunction()

function(add_tool)
  set(options INCLUDE_CURRENT_DIR)
  set(oneValueArgs)
  set(multiValueArgs LIBS SRCS)
  cmake_parse_arguments(TOOL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  set(TOOL_NAME ${TOOL_UNPARSED_ARGUMENTS})

  foreach(FILE ${TOOL_SRCS})
    string(REGEX MATCH "\.cpp\.in$" MATCH "${FILE}")
    if (MATCH)
      string(REGEX REPLACE "\.in$" "" SOURCE "${FILE}")
      configure_file("${FILE}" "${SOURCE}" @ONLY) 
      list(REMOVE_ITEM TOOL_SRCS "${FILE}")
      list(APPEND TOOL_SRCS "${CMAKE_CURRENT_BINARY_DIR}/${SOURCE}")
    endif()
  endforeach()

  dump_variable(
    TOOL_NAME
    TOOL_INCLUDE_CURRENT_DIR
    TOOL_LIBS
    TOOL_SRCS
  )
  add_executable(${TOOL_NAME} ${TOOL_SRCS})
  if (TOOL_INCLUDE_CURRENT_DIR)
    target_include_directories(${TOOL_NAME} BEFORE PRIVATE 
      ${CMAKE_CURRENT_SOURCE_DIR} 
      ${CMAKE_CURRENT_BINARY_DIR}
    )
  endif()
  set_target_properties(${TOOL_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
  target_link_libraries(${TOOL_NAME} ${TOOL_LIBS})
  install(TARGETS ${TOOL_NAME} RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT runtime)
endfunction()
