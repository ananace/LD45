add_library(EnTT INTERFACE)

target_include_directories(EnTT
    INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/entt/src"
)

add_library(GSL INTERFACE)

target_include_directories(GSL
    INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/GSL/include"
)
