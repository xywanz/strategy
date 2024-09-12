gen_param_header_file(strategy_example)
add_library(strategy_example SHARED
    strategy_example.cpp
    strategy_example_param_manager.h)
