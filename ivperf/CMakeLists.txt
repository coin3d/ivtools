set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(BarChart SHARED BarChart.cpp)
target_link_libraries(BarChart Coin::Coin)

configure_file(ivperf.cpp.in ivperf.cpp @ONLY)
add_executable(ivperf ${CMAKE_CURRENT_BINARY_DIR}/ivperf.cpp)
target_link_libraries(ivperf BarChart So${Gui}::So${Gui})
