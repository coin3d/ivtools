macro(coin_project PROJECT_NAME)
  set(options)
  set(oneValueArgs VERSION DESCRIPTION)
  set(multiValueArgs)
  cmake_parse_arguments(PROJECT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
  string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
  string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
  if (${CMAKE_VERSION} VERSION_LESS "3.9") 
    # 'project' command doesn't support DESCRIPTION, so the value parsed with
    # cmake_parse_arguments() will be used
    project(${PROJECT_NAME} VERSION ${PROJECT_VERSION})
  else()
    # up to v3.13.5 if DESCRIPTION is not passed to 'project' command, its 
    # invocation will reset PROJECT_DESCRIPTION
    # from v3.14 the behaviour changes and if not passed, 'project' leaves any
    # already set value untouched
    # so we pass to 'project' the value parsed by 'cmake_parse_arguments'
    project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} DESCRIPTION ${PROJECT_DESCRIPTION})
  endif()
  # ############################################################################
  # GUI target preliminary setup
  set(Gui "Qt" CACHE STRING "Target GUI for the Open Inventor examples")
  set(GuiValues "Qt;Xt;Win" CACHE INTERNAL "List of possible values for the GUI cache variable")
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
  else()
    message(FATAL_ERROR "Only Qt, Xt, and Win are supported: please set Gui to one of these values")
  endif()
  string(TOUPPER ${Gui} GUI)
  # ############################################################################
  string(TIMESTAMP PROJECT_BUILD_YEAR "%Y")
endmacro(coin_project)

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
  target_link_libraries(${TOOL_NAME} ${TOOL_LIBS})
  if (${CMAKE_VERSION} VERSION_LESS "3.14") 
    # DESTINATION is mandatory
    install(TARGETS ${TOOL_NAME} RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT runtime)
  else()
    # DESTINATION is defaulted according to its target type (RUNTIME|etc. etc)
    install(TARGETS ${TOOL_NAME} RUNTIME COMPONENT runtime)
  endif()
endfunction(add_tool)
