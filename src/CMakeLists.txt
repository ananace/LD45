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
    BaseSystem.hpp
    DebugView.hpp
    EntityManager.hpp
    Math.hpp
    ResourceManager.hpp
    Spinor.hpp
    StarSystemGenerator.hpp
    StateManager.hpp
    Util.hpp

    Components/Tags/CameraTag.hpp
    Components/Tags/InFaction.hpp
    Components/Tags/InSystem.hpp
    Components/Tags/JumpCapable.hpp
    Components/Tags/SystemCore.hpp
    Components/Tags/TracedOrbit.hpp

    Components/Acceleration.hpp
    Components/Atmosphere.hpp
    Components/CelestialBody.hpp
    Components/Colony.hpp
    Components/DelayedAction.hpp
    Components/Friction.hpp
    Components/GateShape.hpp
    Components/GravityWell.hpp
    Components/HoleShape.hpp
    Components/JumpConnection.hpp
    Components/PlanetShape.hpp
    Components/PlayerInput.hpp
    Components/Position.hpp
    Components/Renderables.hpp
    Components/Rotation.hpp
    Components/SatteliteBody.hpp
    Components/StarField.hpp
    Components/StarShape.hpp
    Components/StationShape.hpp
    Components/TradeJump.hpp
    Components/UIButton.hpp
    Components/UIComponent.hpp
    Components/UIDialog.hpp
    Components/Velocity.hpp
    Components/VisibleVelocity.hpp

    Ships/BasicShip.hpp

    States/GameState.hpp
    States/MenuState.hpp

    Systems/CelestialRenderSystem.hpp
    Systems/InputSystem.hpp
    Systems/LogicSystem.hpp
    Systems/OrbitalSystem.hpp
    Systems/PhysicsSystem.hpp
    Systems/RenderLerpSystem.hpp
    Systems/RenderSystem.hpp
    Systems/UIRenderSystem.hpp
)

set(LD45_SOURCES
    Application.cpp
    BaseState.cpp
    BaseSystem.cpp
    DebugView.cpp
    EntityManager.cpp
    ResourceManager.cpp
    StarSystemGenerator.cpp
    StateManager.cpp

    Ships/BasicShip.cpp

    States/GameState.cpp
    States/MenuState.cpp

    Systems/CelestialRenderSystem.cpp
    Systems/InputSystem.cpp
    Systems/LogicSystem.cpp
    Systems/OrbitalSystem.cpp
    Systems/PhysicsSystem.cpp
    Systems/RenderLerpSystem.cpp
    Systems/RenderSystem.cpp
    Systems/UIRenderSystem.cpp

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

install(
    TARGETS
    LD45
    RUNTIME DESTINATION bin
)
