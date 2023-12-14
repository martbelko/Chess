add_library(TINY_OBJ_LOADER_LIB STATIC ${CMAKE_CURRENT_LIST_DIR}/src/Empty.cpp)

target_link_libraries(TINY_OBJ_LOADER_LIB PUBLIC tinyobjloader)

add_subdirectory(ext/tinyobjloader)

target_include_directories(TINY_OBJ_LOADER_LIB PUBLIC "${CMAKE_CURRENT_LIST_DIR}/ext/")
