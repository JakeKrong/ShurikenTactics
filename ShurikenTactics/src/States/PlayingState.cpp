#include "PlayingState.h"
#include "Game.h"
#include "Components.h"

#include <functional>
#include <random>
#include <iostream>

PlayingState::PlayingState(Game* game) :
	m_Game(game) {
}

void PlayingState::Enter() {  //Initialise Main Menu
	World& world = m_Game->GetWorld();

	//Register Components
	world.RegisterComponent<Transform>();
	world.RegisterComponent<Renderable>();
	world.RegisterComponent<Collider>();
	world.RegisterComponent<Player>();
	world.RegisterComponent<Physics>();
	world.RegisterComponent<Lifetime>();

	//Register Systems
	m_RenderSystem = world.RegisterSystem<RenderSystem>();
	m_InputSystem = world.RegisterSystem<InputSystem>();
	m_ColliderSystem = world.RegisterSystem<ColliderSystem>();
	m_PhysicsSystem = world.RegisterSystem<PhysicsSystem>();
	m_LifetimeSystem = world.RegisterSystem<LifetimeSystem>();

	//Set Systems World
	m_RenderSystem->SetWorld(&world);
	m_InputSystem->SetWorld(&world);
	m_ColliderSystem->SetWorld(&world);
	m_PhysicsSystem->SetWorld(&world);
	m_LifetimeSystem->SetWorld(&world);

	//Set System Signature
	Signature renderSignature;
	renderSignature.set(world.GetComponentID<Transform>());
	renderSignature.set(world.GetComponentID<Renderable>());
	world.SetSystemSignature<RenderSystem>(renderSignature);

	Signature inputSignature;
	inputSignature.set(world.GetComponentID<Player>());
	inputSignature.set(world.GetComponentID<Transform>());
	inputSignature.set(world.GetComponentID<Renderable>());
	world.SetSystemSignature<InputSystem>(inputSignature);

	Signature colliderSignature;
	colliderSignature.set(world.GetComponentID<Transform>());
	colliderSignature.set(world.GetComponentID<Collider>());
	world.SetSystemSignature<ColliderSystem>(colliderSignature);

	Signature physicsSignature;
	physicsSignature.set(world.GetComponentID<Transform>());
	physicsSignature.set(world.GetComponentID<Physics>());
	world.SetSystemSignature<PhysicsSystem>(physicsSignature);

	Signature lifetimeSignature;
	lifetimeSignature.set(world.GetComponentID<Lifetime>());
	world.SetSystemSignature<LifetimeSystem>(lifetimeSignature);

	// *** Set Entites *** //
	//Background
	Entity background = world.CreateEntity();
	world.AddComponentToEntity<Transform>(background, { {0.0f,0.0f} });
	world.AddComponentToEntity<Renderable>(background, { { 1280, 720 }, true, &m_Game->m_TextureManager.Load("Dojo_Background.png"), false });

	//Floor
	Entity floor = world.CreateEntity();
	world.AddComponentToEntity<Transform>(floor, { {0.0f,680} });
	world.AddComponentToEntity<Renderable>(floor, { { 1300.0f, 50 }, true, &m_Game->m_TextureManager.Load("Wood_Floor.png"), false });
	sf::FloatRect floorHitbox{ {0.0f,680},{ 1300.0f, 50 } };
	world.AddComponentToEntity<Collider>(floor, {{floorHitbox}, ColliderType::ObstacleBox });

	//Ceiling
	Entity ceiling = world.CreateEntity();
	world.AddComponentToEntity<Transform>(ceiling, { {0.0f,0} });
	world.AddComponentToEntity<Renderable>(ceiling, { { 1300.0f, 50 }, true, &m_Game->m_TextureManager.Load("Wood_Floor.png"), false });
	sf::FloatRect floor2Hitbox{ {0.0f,0},{ 1300.0f, 50 } };
	world.AddComponentToEntity<Collider>(ceiling, { {floor2Hitbox}, ColliderType::ObstacleBox });

	//Wall
	Entity wallLeft = world.CreateEntity();
	world.AddComponentToEntity<Transform>(wallLeft, { {0.0f,0.0f} });
	world.AddComponentToEntity<Renderable>(wallLeft, { { 50, 800 }, true, &m_Game->m_TextureManager.Load("Wooden_Pillar.png"), false });
	sf::FloatRect wallLeftHitbox{ {0.0f,0},{ 50, 800 } };
	world.AddComponentToEntity<Collider>(wallLeft, { {wallLeftHitbox}, ColliderType::ObstacleBox });

	Entity wallRight = world.CreateEntity();
	world.AddComponentToEntity<Transform>(wallRight, { {1230.0f,0.0f} });
	world.AddComponentToEntity<Renderable>(wallRight, { { 50, 800 }, true, &m_Game->m_TextureManager.Load("Wooden_Pillar.png"), false });
	sf::FloatRect wallRightHitBox{ {1230.0f,0},{ 50, 800 } };
	world.AddComponentToEntity<Collider>(wallRight, { {wallRightHitBox}, ColliderType::ObstacleBox });

	//Extra Platforms
	Entity platform1 = world.CreateEntity();
	world.AddComponentToEntity<Transform>(platform1, { {0.0f,500.0f} });
	world.AddComponentToEntity<Renderable>(platform1, { { 600.0f, 50 }, true, &m_Game->m_TextureManager.Load("Wood_Floor.png"), false });
	sf::FloatRect platform1Hitbox{ {0.0f,500.0f},{ 600.0f, 50 } };
	world.AddComponentToEntity<Collider>(platform1, { {platform1Hitbox}, ColliderType::ObstacleBox });

	Entity platform2 = world.CreateEntity();
	world.AddComponentToEntity<Transform>(platform2, { {700.0f,200.0f} });
	world.AddComponentToEntity<Renderable>(platform2, { { 800.0f, 50 }, true, &m_Game->m_TextureManager.Load("Wood_Floor.png"), false });
	sf::FloatRect platform2Hitbox{ {700.0f,200.0f},{ 800.0f, 50 } };
	world.AddComponentToEntity<Collider>(platform2, { {platform2Hitbox}, ColliderType::ObstacleBox });

	Entity platform3 = world.CreateEntity();
	world.AddComponentToEntity<Transform>(platform3, { {0.0f,300.0f} });
	world.AddComponentToEntity<Renderable>(platform3, { { 300.0f, 50 }, true, &m_Game->m_TextureManager.Load("Wood_Floor.png"), false });
	sf::FloatRect platform3Hitbox{ {0.0f,300.0f},{ 300.0f, 50 } };
	world.AddComponentToEntity<Collider>(platform3, { {platform3Hitbox}, ColliderType::ObstacleBox });


	//Test target
	SpawnTarget();

	//Player
	Entity player = world.CreateEntity();
	world.AddComponentToEntity<Transform>(player, { {400.0f,500.0f} });
	world.AddComponentToEntity<Renderable>(player, { { 60, 100 }, true, &m_Game->m_TextureManager.Load("Player_Idle_Sprite.png"), true, {4,1}, 4, 0.15f });
	world.AddComponentToEntity<Player>(player, {100});
	sf::FloatRect playerHitbox{ {400.0f,100.0f},{ 60, 100 } };
	world.AddComponentToEntity<Collider>(player, { { playerHitbox }, ColliderType::PlayerBox });
	world.AddComponentToEntity<Physics>(player, { { 0.0f, 0.0f }, { 0.0f, 0.0f }, 100.0, true});
}

void PlayingState::Exit() {
	std::println("[PlayingState] Exiting...");

	m_RenderSystem.reset();
	m_InputSystem.reset();
	m_PhysicsSystem.reset();
	m_ColliderSystem.reset();

	m_Game->m_SoundManager.StopAll();
	m_Game->GetWorld().ResetManagers();
}

void PlayingState::Update(sf::RenderWindow& renderWindow, const float& deltaTime) {
	//Systems Update
	m_LifetimeSystem->Update(deltaTime);
	m_PhysicsSystem->Update(deltaTime);
	m_ColliderSystem->Update();
	m_InputSystem->Update(renderWindow);

	UpdatePlayerState();

	//Update variables
	if (shurikenCD > 0) {
		shurikenCD -= deltaTime;
		if (shurikenCD <= 0) m_InputSystem->SetKeyboardDisabled(false);
	}
}

void PlayingState::Render(sf::RenderWindow& renderWindow, const float& deltaTime) {
	m_RenderSystem->Update(renderWindow, deltaTime);
}

void PlayingState::HandleEvents(const sf::Event& event) {

}

void PlayingState::UpdatePlayerState() {
	if (m_InputSystem->WasLeftClicked() && shurikenCD <= 0.0f) 
		ThrowShuriken(m_Game->GetWindow().mapPixelToCoords(m_InputSystem->GetMousePosition()));
	else if(shurikenCD <= 0) {
		Entity playerEnt = *m_InputSystem->ReturnEntities().begin();
		World& world = m_Game->GetWorld();
		Renderable& playerRenderable = world.GetComponent<Renderable>(playerEnt);

		if (m_InputSystem->m_A_KeyPressed || m_InputSystem->m_D_KeyPressed) {
			if (playerRenderable.texture != &m_Game->m_TextureManager.Load("Player_Sprint_Sprite.png")) {
				playerRenderable.texture = &m_Game->m_TextureManager.Load("Player_Sprint_Sprite.png");
				playerRenderable.spriteSheetDim = { 6,1 };
				playerRenderable.totalFrames = 6;
				playerRenderable.timeSinceLastFrame = 0.15f;
			}
		}
		else {
			if (playerRenderable.texture != &m_Game->m_TextureManager.Load("Player_Idle_Sprite.png")) {
				playerRenderable.texture = &m_Game->m_TextureManager.Load("Player_Idle_Sprite.png");
				playerRenderable.spriteSheetDim = { 4,1 };
				playerRenderable.totalFrames = 4;
				playerRenderable.timeSinceLastFrame = 0.15f;
			}
		}
	}
}

void PlayingState::ThrowShuriken(sf::Vector2f mousePos){
	Entity playerEnt = *m_InputSystem->ReturnEntities().begin();
	World& world = m_Game->GetWorld();
	sf::FloatRect playerBox{ {world.GetComponent<Transform>(playerEnt).position},
						 {world.GetComponent<Renderable>(playerEnt).size} };

	Entity shuriken = world.CreateEntity();
	world.AddComponentToEntity<Transform>(shuriken, { playerBox.getCenter() });
	std::function<void(Transform&)> transFunc = [](Transform& trans) {trans.rotation += 10; }; //Function to rotate shuriken
	world.AddComponentToEntity<Renderable>(shuriken, { {30.0f, 30.0f}, true, &m_Game->m_TextureManager.Load("Shuriken.png"), true, {3,1}, 3, 0.03f });

	sf::Vector2f shurikenDir = mousePos - playerBox.getCenter();
	sf::Vector2f normalizedDir = shurikenDir / std::sqrt(shurikenDir.x * shurikenDir.x + shurikenDir.y * shurikenDir.y);
	world.AddComponentToEntity<Physics>(shuriken, { {normalizedDir.x * 1500, normalizedDir.y * 1500 } });

	sf::CircleShape shurikenHitbox{ 15.0f };
	world.AddComponentToEntity<Collider>(shuriken, { {shurikenHitbox}, ColliderType::ProjectileBox });
	
	std::function<void(Entity ent)> onShurikenDestroyed = [this] (Entity shuriken){
		World& world = m_Game->GetWorld();
		Entity destroyEffect = world.CreateEntity();
		world.AddComponentToEntity<Transform>(destroyEffect, world.GetComponent<Transform>(shuriken));
		world.AddComponentToEntity<Renderable>(destroyEffect, { {30.0f, 30.0f}, true, &m_Game->m_TextureManager.Load("Shuriken_Break.png"), true, {3,1}, 3, 0.05f } );
		world.AddComponentToEntity<Lifetime>(destroyEffect, { 0.15f ,0 });
		};
	world.AddComponentToEntity<Lifetime>(shuriken, { 0,3, onShurikenDestroyed });

	Renderable& playerRenderable = world.GetComponent<Renderable>(playerEnt);
	if (playerRenderable.texture != &m_Game->m_TextureManager.Load("Player_Throw_Sprite.png")) {
		playerRenderable.texture = &m_Game->m_TextureManager.Load("Player_Throw_Sprite.png");
		playerRenderable.spriteSheetDim = { 2,1 };
		playerRenderable.totalFrames = 2;
		playerRenderable.timeSinceLastFrame = 0.15f;
	}
	//Refresh cooldown
	shurikenCD += 0.30f;
	m_InputSystem->SetKeyboardDisabled(true);
}

void PlayingState::SpawnTarget() {
	World& world = m_Game->GetWorld();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> disX(100, 1100);
	std::uniform_int_distribution<> disY(50, 650);

	float spawnX = static_cast<float>(disX(gen));
	float spawnY = static_cast<float>(disY(gen));

	Entity target = world.CreateEntity();
	world.AddComponentToEntity<Transform>(target, { {spawnX, spawnY} });

	world.AddComponentToEntity<Renderable>(target, { { 60, 100 }, true, &m_Game->m_TextureManager.Load("Strawman_Target.png") });
	sf::FloatRect targetHitbox{ {spawnX, spawnY},{ 60, 100 } };
	world.AddComponentToEntity<Collider>(target, { { targetHitbox }, ColliderType::TargetBox });
	world.AddComponentToEntity<Physics>(target, { { 0.0f, 0.0f }, { 0.0f, 0.0f }, 100.0, true });
	std::function<void(Entity ent)> onTargetDestroyed = [this](Entity target) {
		World& world = m_Game->GetWorld();
		Entity destroyEffect = world.CreateEntity();
		world.AddComponentToEntity<Transform>(destroyEffect, world.GetComponent<Transform>(target));
		sf::Vector2f& targetSize = world.GetComponent<Renderable>(target).size;
		world.AddComponentToEntity<Renderable>(destroyEffect, { {targetSize.x, targetSize.y}, true, &m_Game->m_TextureManager.Load("Explosion.png"), true, {5,2}, 6, 0.15f });
		world.AddComponentToEntity<Lifetime>(destroyEffect, { 0.9f ,0 });
		SpawnTarget();
		};
	world.AddComponentToEntity<Lifetime>(target, { 0,1, onTargetDestroyed });

	Entity spawnSmoke = world.CreateEntity();
	world.AddComponentToEntity<Transform>(spawnSmoke, { {spawnX, spawnY} });
	world.AddComponentToEntity<Renderable>(spawnSmoke, { { 60, 100 }, true, &m_Game->m_TextureManager.Load("Smoke.png"), true, {6,1}, 6, 0.1f });
	world.AddComponentToEntity<Lifetime>(spawnSmoke, { 0.5f,0 });
}