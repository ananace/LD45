cmake_minimum_required(VERSION 3.12...3.15)

project(LD45
    DESCRIPTION "Ludum Dare #45: Untitled Space Exploration Game."
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_DEBUG_POSTFIX          "d")
set(CMAKE_MINSIZEREL_POSTFIX     "s")
set(CMAKE_RELWITHDEBINFO_POSTFIX "rd")

set(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${OUTPUT_BINDIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BINDIR})
foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER "${CONF}" CONF)
    set("CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}")
endforeach ()

add_subdirectory(extern)
add_subdirectory(src)

if (UNIX)
    configure_file(dist/com.ldjam.ananace.LD45.desktop.in dist/com.ldjam.ananace.LD45.desktop)
    configure_file(dist/LD45.sh.in dist/LD45)

    install(
        PROGRAMS
        ${CMAKE_CURRENT_BINARY_DIR}/dist/LD45
        DESTINATION bin
    )
    install(
        FILES
        ${CMAKE_CURRENT_BINARY_DIR}/dist/com.ldjam.ananace.LD45.desktop
        DESTINATION share/applications
    )

    install(
        FILES
        data/Atmosphere.frag
        data/Corona.frag
        data/Orbit.frag
        DESTINATION share/AnanaceLD45/data
    )

    install(
        FILES
        dist/com.ldjam.ananace.LD45.png
        DESTINATION share/icons/hicolor/512x512/apps
    )
endif()
