add_library(my_data_filter SHARED my_data_filter.cpp)

add_library(my_matching_engine SHARED my_matching_engine.cpp)

add_library(my_data_replayer SHARED my_data_replayer.cpp)

add_library(my_collected_data_handler SHARED my_collected_data_handler.cpp)

# require boost
# sudo apt install libboost-all-dev
# add_library(my_wechat_sender SHARED my_wechat_sender.cpp)
