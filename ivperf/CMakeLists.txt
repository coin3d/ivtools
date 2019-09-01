add_library(BarChart STATIC BarChart.cpp BarChart.h)
target_link_libraries(BarChart Coin::Coin)
target_include_directories(BarChart BEFORE PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

add_tool(ivperf INCLUDE_CURRENT_DIR SRCS ivperf.cpp.in LIBS ${IVTOOLS_ADDITIONAL_LIBRARIES} BarChart So${Gui}::So${Gui})
