# CMakeList.txt : CMake project for ShurikenTactics, include source and define project specific logic here.

add_executable (ShurikenTactics 
"src/ShurikenTactics.cpp" "src/ShurikenTactics.h" "src/Core/Types.h" 
"src/Core/EntityManager.h" "src/Core/EntityManager.cpp" 
"src/Core/ComponentManager.h"
"src/Components/Transform.h" "src/Components/Renderable.h" "src/Components/Physics.h" "src/Components/Player.h" 
"src/Components/Components.h" "src/Components/Button.h""src/Components/Collider.h" "src/Components/Lifetime.h"
"src/Core/SystemManager.h" "src/Systems/ISystem.h"
"src/Systems/RenderSystem.h"  "src/Systems/RenderSystem.cpp" 
"src/Systems/ColliderSystem.h" "src/Systems/ColliderSystem.cpp" 
"src/Systems/InputSystem.h" "src/Systems/InputSystem.cpp" "src/Systems/UISystem.h" "src/Systems/UISystem.cpp" "src/Systems/LifetimeSystem.h" "src/Systems/LifetimeSystem.cpp"
"src/Systems/PhysicsSystem.h" "src/Systems/PhysicsSystem.cpp"
"src/Core/World.h" "src/Core/World.cpp"
"src/Core/Game.h" "src/Core/Game.cpp"
"src/Assets/TextureManager.h" "src/Assets/TextureManager.cpp" "src/Assets/FontManager.h" "src/Assets/FontManager.cpp" "src/Assets/SoundManager.h" "src/Assets/SoundManager.cpp" 
"src/Core/StateManager.h" "src/Core/StateManager.cpp" 
"src/States/IState.h" "src/States/MainMenuState.h" "src/States/MainMenuState.cpp" "src/States/PlayingState.h" "src/States/PlayingState.cpp")

target_include_directories(ShurikenTactics PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Core
	${CMAKE_CURRENT_SOURCE_DIR}/src/Components
	${CMAKE_CURRENT_SOURCE_DIR}/src/Systems
	${CMAKE_CURRENT_SOURCE_DIR}/src/Assets
	${CMAKE_CURRENT_SOURCE_DIR}/src/States
	${CMAKE_CURRENT_SOURCE_DIR}/assets
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ShurikenTactics PROPERTY CXX_STANDARD 23)
endif()

# TODO: Add tests and install targets if needed.

target_link_libraries(ShurikenTactics PRIVATE
	sfml-graphics
	sfml-window
	sfml-system
	sfml-audio
)
