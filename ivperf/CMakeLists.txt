add_library(BarChart SHARED BarChart.cpp)
target_link_libraries(BarChart Coin::Coin)

add_tool(ivperf INCLUDE_CURRENT_DIR SRCS ivperf.cpp.in LIBS BarChart So${Gui}::So${Gui})
