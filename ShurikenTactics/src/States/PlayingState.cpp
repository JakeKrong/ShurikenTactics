#include "PlayingState.h"
#include "Game.h"
#include "Components.h"
#include "Prefabs.h"

#include <functional>
#include <random>
#include <iostream>

PlayingState::PlayingState(Game* game) :
	m_Game(game) {
}

//Initialise Stage upon entering
void PlayingState::Enter() {  
	World& world = m_Game->GetWorld();

	//Register Components
	world.RegisterComponent<Transform>();
	world.RegisterComponent<Renderable>();
	world.RegisterComponent<AnimationData>();
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

	PrefabGen::game = m_Game;
	m_Game->GetTextureManager().InitGameAnimationData();

	// *** Set Entites *** //
	//Background
	Entity background = world.CreateEntity();
	world.AddComponentToEntity<Transform>(background, { {0.0f,0.0f} });
	world.AddComponentToEntity<Renderable>(background, { { 1280, 720 }, RenderLayer::Background, true, &m_Game->m_TextureManager.Load("Dojo_Background")});

	//Floor
	PrefabGen::Platform({ 0,680 }, { 1300,50 });

	//Ceiling
	PrefabGen::Platform({ 0,0 }, { 1300,50 });

	//Walls
	PrefabGen::Wall({ 0,0 }, { 50, 800 });
	PrefabGen::Wall({ 1230,0 }, { 50, 800 });

	//Extra Platforms
	PrefabGen::Platform({ 0,500 }, { 600,50 });
	PrefabGen::Platform({ 700,200 }, { 800,50 });
	PrefabGen::Platform({ 0,300 }, { 300,50 });

	//Test target
	SpawnTarget();

	//Player
	PrefabGen::Player();
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
	if (event.is< sf::Event::FocusLost >()) {
		m_InputSystem->SetWindowFocused(false);
	}
	else if (event.is < sf::Event::FocusGained>()) {
		m_InputSystem->SetWindowFocused(true);
	}
}

void PlayingState::UpdatePlayerState() {
	if (m_InputSystem->WasLeftClicked() && shurikenCD <= 0.0f) 
		ThrowShuriken(m_Game->GetWindow().mapPixelToCoords(m_InputSystem->GetMousePosition()));
	else if(shurikenCD <= 0) {
		Entity playerEnt = *m_InputSystem->ReturnEntities().begin();
		World& world = m_Game->GetWorld();
		Renderable& playerRenderable = world.GetComponent<Renderable>(playerEnt);
		AnimationData& playerAnimation = world.GetComponent<AnimationData>(playerEnt);

		if (m_InputSystem->m_A_KeyPressed || m_InputSystem->m_D_KeyPressed) {
			if (playerRenderable.texture != &m_Game->m_TextureManager.Load("Player_Sprint_Sprite")) {
				playerRenderable.texture = &m_Game->m_TextureManager.Load("Player_Sprint_Sprite");
				m_Game->m_TextureManager.SetAnimationData("Player_Sprint_Sprite", playerAnimation);
			}
		}
		else {
			if (playerRenderable.texture != &m_Game->m_TextureManager.Load("Player_Idle_Sprite")) {
				playerRenderable.texture = &m_Game->m_TextureManager.Load("Player_Idle_Sprite");
				m_Game->m_TextureManager.SetAnimationData("Player_Idle_Sprite", playerAnimation);
			}
		}
	}
}

void PlayingState::ThrowShuriken(sf::Vector2f mousePos){
	Entity playerEnt = *m_InputSystem->ReturnEntities().begin();
	World& world = m_Game->GetWorld();
	sf::FloatRect playerBox{ {world.GetComponent<Transform>(playerEnt).position},
						 {world.GetComponent<Renderable>(playerEnt).size} };

	Entity shuriken = PrefabGen::Shuriken();

	world.GetComponent<Transform>(shuriken) = { playerBox.getCenter() };

	sf::Vector2f shurikenDir = mousePos - playerBox.getCenter();
	sf::Vector2f normalizedDir = shurikenDir / std::sqrt(shurikenDir.x * shurikenDir.x + shurikenDir.y * shurikenDir.y);
	world.GetComponent<Physics>(shuriken) = { {normalizedDir.x * 1000, normalizedDir.y * 1000 } };

	Renderable& playerRenderable = world.GetComponent<Renderable>(playerEnt);
	AnimationData& playerAnimation = world.GetComponent<AnimationData>(playerEnt);

	playerRenderable.texture = &m_Game->m_TextureManager.Load("Player_Throw_Sprite");
	m_Game->m_TextureManager.SetAnimationData("Player_Throw_Sprite", playerAnimation);

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

	Entity target = PrefabGen::Target();
	world.GetComponent<Transform>(target) = { {spawnX, spawnY} };

	std::function<void(Entity ent)> onTargetDestroyed = [this](Entity target) {
		World& world = m_Game->GetWorld();
		Entity destroyEffect = world.CreateEntity();
		world.AddComponentToEntity<Transform>(destroyEffect, world.GetComponent<Transform>(target));
		sf::Vector2f& targetSize = world.GetComponent<Renderable>(target).size;
		world.AddComponentToEntity<Renderable>(destroyEffect, { {targetSize.x, targetSize.y}, RenderLayer::GameObject1, true, &m_Game->m_TextureManager.Load("Explosion") });
		world.AddComponentToEntity<AnimationData>(destroyEffect, { {5,2}, 10, 0.05f });
		world.AddComponentToEntity<Lifetime>(destroyEffect, { 0.45f ,0 });
		SpawnTarget();
		};
	world.GetComponent<Lifetime>(target).OnDestroyedFunction = onTargetDestroyed;

	Entity spawnSmoke = world.CreateEntity();
	world.AddComponentToEntity<Transform>(spawnSmoke, { {spawnX, spawnY} });
	world.AddComponentToEntity<Renderable>(spawnSmoke, { { 60, 100 }, RenderLayer::GameObject1, true, &m_Game->m_TextureManager.Load("Smoke") });
	world.AddComponentToEntity<AnimationData>(spawnSmoke, { {6,1}, 6, 0.1f });
	world.AddComponentToEntity<Lifetime>(spawnSmoke, { 0.5f,0 });
}