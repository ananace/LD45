# CMake config
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Dependency discovery
if (WIN32)
    find_package(SFML 2 REQUIRED COMPONENTS audio graphics main system window)
    add_definitions(-D_USE_MATH_DEFINES)
else ()
    find_package(SFML 2 REQUIRED COMPONENTS audio graphics system window)
endif ()

# Sources
set(LD45_HEADERS
    Application.hpp
    BaseState.hpp
    DebugView.hpp
    StateManager.hpp
    Util.hpp
)

set(LD45_SOURCES
    Application.cpp
    BaseState.cpp
    DebugView.cpp
    StateManager.cpp

    main.cpp
)

list(TRANSFORM LD45_HEADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
list(TRANSFORM LD45_SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Header Files"
    FILES ${LD45_HEADERS}
)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Source Files"
    FILES ${LD45_SOURCES}
)

add_executable(LD45 ${LD45_HEADERS} ${LD45_SOURCES})
set_property(TARGET LD45 PROPERTY FOLDER "LD45")

target_link_libraries(LD45
    PRIVATE EnTT GSL sfml-system sfml-audio sfml-window sfml-graphics
)
target_include_directories(LD45
    PRIVATE
)
