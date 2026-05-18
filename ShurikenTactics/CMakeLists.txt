# CMakeList.txt : CMake project for ShurikenTactics, include source and define project specific logic here.

set (GAME_SOURCES 
"src/ShurikenTactics.h" 
"src/Core/Types.h" 
"src/Core/EntityManager.h"	"src/Core/EntityManager.cpp" 
"src/Core/ComponentManager.h"
"src/Core/World.h"			"src/Core/World.cpp"
"src/Core/Game.h"			"src/Core/Game.cpp"
"src/Core/SystemManager.h" 
"src/Core/StateManager.h"	"src/Core/StateManager.cpp" 
"src/Core/LevelGenerator.h" "src/Core/LevelGenerator.cpp"
"src/Core/Prefabs.h"

"src/Components/Transform.h"	"src/Components/Renderable.h"	"src/Components/Physics.h"	"src/Components/Player.h" 
"src/Components/Components.h"	"src/Components/Button.h"		"src/Components/Collider.h"	"src/Components/Lifetime.h"
"src/Components/Enemy.h"

"src/Systems/ISystem.h"
"src/Systems/RenderSystem.h"	"src/Systems/RenderSystem.cpp" 
"src/Systems/ColliderSystem.h"	"src/Systems/ColliderSystem.cpp" 
"src/Systems/InputSystem.h"		"src/Systems/InputSystem.cpp" 
"src/Systems/UISystem.h"		"src/Systems/UISystem.cpp" 
"src/Systems/LifetimeSystem.h"	"src/Systems/LifetimeSystem.cpp"
"src/Systems/PhysicsSystem.h"	"src/Systems/PhysicsSystem.cpp"
"src/Systems/EnemySystem.h"		"src/Systems/EnemySystem.cpp"

"src/Assets/TextureManager.h"	"src/Assets/TextureManager.cpp" 
"src/Assets/FontManager.h"		"src/Assets/FontManager.cpp" 
"src/Assets/SoundManager.h"		"src/Assets/SoundManager.cpp" 

"src/States/IState.h"			
"src/States/MainMenuState.h" "src/States/MainMenuState.cpp"
"src/States/PlayingState.h" "src/States/PlayingState.cpp" 

 "src/Gameplay/TrajectoryMath.h" "src/Gameplay/TrajectoryMath.cpp")

# Main Executable
add_executable(ShurikenTactics "src/ShurikenTactics.cpp" ${GAME_SOURCES})

# Test Executable
add_executable(ShurikenTests "tests/test_main.cpp"
	"tests/Trajectory.spec.cpp"
	${GAME_SOURCES}
 )

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Core
	${CMAKE_CURRENT_SOURCE_DIR}/src/Components
	${CMAKE_CURRENT_SOURCE_DIR}/src/Systems
	${CMAKE_CURRENT_SOURCE_DIR}/src/Assets
	${CMAKE_CURRENT_SOURCE_DIR}/src/States
	${CMAKE_CURRENT_SOURCE_DIR}/src/Gameplay
)

target_include_directories(ShurikenTactics PRIVATE ${INCLUDE_DIRS})
target_include_directories(ShurikenTests PRIVATE ${INCLUDE_DIRS})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ShurikenTactics PROPERTY CXX_STANDARD 23)
  set_property(TARGET ShurikenTests PROPERTY CXX_STANDARD 23)
endif()

# TODO: Add tests and install targets if needed.

target_link_libraries(ShurikenTactics PRIVATE
	sfml-graphics
	sfml-window
	sfml-system
	sfml-audio
)

target_link_libraries(ShurikenTests PRIVATE
	sfml-graphics
	sfml-window
	sfml-system
	sfml-audio
	Catch2::Catch2WithMain
)