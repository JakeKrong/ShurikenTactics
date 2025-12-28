#include "LevelGenerator.h"
#include "Prefabs.h"

bool LevelGenerator::LoadLevel(int level) {
	World& world = m_Game->GetWorld();
	if (!PrefabGen::game) PrefabGen::Init(m_Game);

	CreateLevelBorders();
	CreateControlsDisplay();
	CreateStartingDoors();

	switch (level) {
	case 1:
		//Floors & Platforms
		PrefabGen::Floor({ 0, 170 }, { 900,50 });
		PrefabGen::Platform({ 900, 170 }, { 150, 25 });
		PrefabGen::Floor({ 1050, 170 }, { 180,50 });

		PrefabGen::Floor({ 50, 370 }, { 370,50 });
		PrefabGen::Floor({ 540, 370 }, { 690,50 });
		PrefabGen::Platform({ 420, 370 }, { 120, 20 });

		PrefabGen::Wall({ 370, 370 }, { 50, 150 });
		PrefabGen::Wall({ 540, 370 }, { 50, 150 });
		PrefabGen::Platform({ 420, 500 }, { 120, 20 });

		PrefabGen::Player({ 100, 100 });

		//Enemies
		PrefabGen::Samurai({ 780, 350 }, false);
		PrefabGen::Samurai({ 150, 350 }, true);
		PrefabGen::Samurai({ 1000, 550 }, false);

		return true;

	case 2:
		//Floors & Platforms

		PrefabGen::Wall({ 780, 50 }, { 50, 110 });
		PrefabGen::Floor({ 1050, 200 }, { 180, 50 });

		PrefabGen::Floor({ 50, 250 }, { 200, 50 });

		PrefabGen::Platform({ 50, 480 }, { 201, 20 });
		PrefabGen::Wall({ 250, 480 }, { 50, 200 });
		PrefabGen::Floor({ 300, 480 }, { 150, 50 });
		PrefabGen::Platform({ 450, 480 }, { 480, 20 });

		PrefabGen::Floor({ 930, 480 }, { 300, 50 });

		PrefabGen::Player({ 100, 650 });

		//Enemies
		PrefabGen::Archer({ 50, 0 }, true);
		PrefabGen::Archer({ 950, 0 }, false);
		PrefabGen::Samurai({ 200, 550 }, true);
		PrefabGen::Archer({ 1000, 550 }, false);

		return true;

	case 3:
		//Floors & Platforms
		PrefabGen::Floor({ 50, 180 }, { 320,40 });
		PrefabGen::Platform({ 370, 180 }, { 100,20 });
		PrefabGen::Wall({ 470, 50 }, { 50, 165 });
		PrefabGen::Wall({ 800, 50 }, { 50, 90 });
		PrefabGen::SidedPlatform({ 1050, 200 }, { 180,25 });

		PrefabGen::Platform({ 50, 350 }, { 150, 20 });
		PrefabGen::Floor({ 200, 350 }, { 410,50 });
		PrefabGen::Platform({ 610, 350 }, { 100, 20 });
		PrefabGen::Floor({ 710, 350 }, { 520,50 });

		PrefabGen::Wall({ 200, 350 }, { 50, 200 });
		PrefabGen::Wall({ 710, 350 }, { 50, 180 });

		PrefabGen::Platform({ 50, 480 }, { 150, 20 });

		PrefabGen::Player({ 60, 50 });

		//Enemies
		PrefabGen::Samurai({ 70, 230 }, true);
		PrefabGen::Archer({ 950, 0 }, false);
		PrefabGen::Archer({ 850, 100 }, false);

		PrefabGen::Samurai({ 850, 500 }, false);
		PrefabGen::Archer({ 1000, 500 }, false);

		return true;

	case 4:
		//Floors & Platforms

		PrefabGen::Wall({ 550, 50 }, { 50, 50 });

		PrefabGen::Floor({ 50, 50 }, { 180,50 });

		PrefabGen::Floor({ 50, 280 }, { 115,50 });
		PrefabGen::Floor({ 450, 280 }, { 450,50 });
		PrefabGen::Platform({ 900, 280 }, { 130,20 });
		PrefabGen::Floor({ 1030, 280 }, { 200,50 });

		PrefabGen::Floor({ 50, 480 }, { 160,50 });
		PrefabGen::Wall({ 210, 480 }, { 50, 200 });
		PrefabGen::Platform({ 260, 480 }, { 100, 20 });
		PrefabGen::Wall({ 360, 480 }, { 50, 200 });
		PrefabGen::Floor({ 410, 480 }, { 235, 50 });
		PrefabGen::Platform({ 645, 480 }, { 175, 20 });
		PrefabGen::Floor({ 820, 480 }, { 410, 50 });

		PrefabGen::Player({ 250, 600 });

		//Enemies
		PrefabGen::Archer({ 500, 100 }, true);
		PrefabGen::Archer({ 600, 100 }, true);
		PrefabGen::Archer({ 975, 280 }, false);
		PrefabGen::Archer({ 590, 550 }, false);
		PrefabGen::Samurai({ -47, 280 }, true);
		PrefabGen::Samurai({ 1000, 550 }, false);

		return true;

	default:
		//Floors & Platforms
		PrefabGen::Floor({ 0, 170 }, { 900,50 });
		PrefabGen::Platform({ 900, 170 }, { 150, 25 });
		PrefabGen::Floor({ 1050, 170 }, { 180,50 });

		PrefabGen::Floor({ 50, 370 }, { 370,50 });
		PrefabGen::Floor({ 540, 370 }, { 690,50 });
		PrefabGen::Platform({ 420, 370 }, { 120, 20 });

		PrefabGen::Wall({ 370, 370 }, { 50, 150 });
		PrefabGen::Wall({ 540, 370 }, { 50, 150 });
		PrefabGen::Platform({ 420, 500 }, { 120, 20 });

		PrefabGen::Player({ 100, 100 });

		//Enemies
		PrefabGen::Samurai({ 780, 350 }, false);
		PrefabGen::Samurai({ 150, 350 }, true);
		PrefabGen::Samurai({ 1000, 550 }, false);

		return true;
	}

}

void LevelGenerator::CreateLevelBorders() {
	//Background
	Entity background = m_Game->GetWorld().CreateEntity();
	m_Game->GetWorld().AddComponentToEntity<Transform>(background, { {0.0f,0.0f} });
	m_Game->GetWorld().AddComponentToEntity<Renderable>(background, { { 1280, 720 }, RenderLayer::Background, &m_Game->GetTextureManager().Load("Dojo_Background"), true});

	//Walls
	PrefabGen::Wall({ 0,0 }, { 50, 800 });
	PrefabGen::Wall({ 1230,0 }, { 50, 800 });

	//Ceiling & Floor
	PrefabGen::Floor({ 0,0 }, { 1300,50 });
	PrefabGen::Floor({ 0,680 }, { 1300,50 });
}

void LevelGenerator::CreateStartingDoors() {
	World& world = m_Game->GetWorld();
	SoundManager& soundMn = m_Game->GetSoundManager();

	Entity leftDoor = PrefabGen::StaticDoor({0,0}, false);
	world.GetComponent<Lifetime>(leftDoor).OnDestroyedFunction = [&](Entity) {
		PrefabGen::SlidingDoor({ 0,0 }, false, false);
		soundMn.PlaySound("Sliding_Door");
		};

	Entity rightDoor = PrefabGen::StaticDoor({ 640,0 }, true);
	world.GetComponent<Lifetime>(rightDoor).OnDestroyedFunction = [&](Entity) {
		PrefabGen::SlidingDoor({ 640,0 }, true, true);
		};
}

void LevelGenerator::CreateControlsDisplay() {
	World& world = m_Game->GetWorld();

	PrefabGen::Key({ 20 ,687 }, "A_Key");
	PrefabGen::Text({ 55 ,689 }, "Move Left", 17, sf::Color::Black);
	PrefabGen::Key({ 160 ,687 }, "D_Key");
	PrefabGen::Text({ 195 ,689 }, "Move Right", 17, sf::Color::Black);
	PrefabGen::Key({ 300 ,687 }, "W_Key");
	PrefabGen::Text({ 335 ,689 }, "Jump", 17, sf::Color::Black);
	PrefabGen::Key({ 440 ,687 }, "S_Key");
	PrefabGen::Text({ 475 ,689 }, "Drop Down", 17, sf::Color::Black);

	PrefabGen::Key({ 580 ,687 }, "Left_Mouse");
	PrefabGen::Text({ 615 ,689 }, "Aim[Hold] / Shoot[Release]", 17, sf::Color::Black);

	Entity escKey = PrefabGen::Key({ 1100 ,687 }, "Esc_Key");
	world.GetComponent<Renderable>(escKey).size.x *= 2;
	PrefabGen::Text({ 1170 ,689 }, "Pause Game", 17, sf::Color::Black);
}