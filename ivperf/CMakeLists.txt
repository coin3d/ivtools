add_library(BarChart STATIC BarChart.cpp BarChart.h)
target_link_libraries(BarChart Coin::Coin)
target_include_directories(BarChart BEFORE PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

# Replace the IRIX command 'hinv' on the original Silicon Graphics code
#
# system("hinv -c processor"); printf("\n");
# system("hinv -t memory"); printf("\n");
# system("hinv -c graphics"); printf("\n");
#
# with something equivalent for current OS
set(HINV_CMD "echo \\\"no hinv clone found on this system/OS: skip hardware inventory\\\"")
if (WIN32)
  # Windows operating system
  message(WARNING "no hinv clone found on this system/OS: hardware inventory removed")
elseif(APPLE)
  # MacOS X or iOS, watchOS, tvOS (since 3.10.3)
  message(WARNING "no hinv clone found on this system/OS: hardware inventory removed")
elseif (UNIX AND NOT APPLE)
  # Linux, BSD, Solaris, Minix
  find_program(LSHW lshw DOC "Hardware inventory utility")
  if (LSHW)
    set(HINV_CMD "${LSHW} -c processor -c memory -c display -short")
    if(LSHW MATCHES ".*/sbin/.*")
      set(HINV_CMD "sudo ${HINV_CMD}")
      message(WARNING "On this system lshw will run with sudo: user must be authorized")
    endif()
  else()
    message(WARNING "No lshw program found: hardware inventory removed")
  endif()
endif (WIN32)

add_tool(ivperf INCLUDE_CURRENT_DIR SRCS ivperf.cpp.in LIBS ${IVTOOLS_ADDITIONAL_LIBRARIES} BarChart So${Gui}::So${Gui})
