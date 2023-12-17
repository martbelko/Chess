add_library(ENTT_LIB STATIC ${CMAKE_CURRENT_LIST_DIR}/src/Empty.cpp)

target_link_libraries(ENTT_LIB PUBLIC EnTT)

add_subdirectory(ext/entt)

target_include_directories(ENTT_LIB PUBLIC "${CMAKE_CURRENT_LIST_DIR}/ext/entt/single_include/")
