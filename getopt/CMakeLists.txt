add_library(localgetopt STATIC getopt.c getopt.h)
target_include_directories(localgetopt BEFORE PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}
)
