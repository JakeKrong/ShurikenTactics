#include "PlayingState.h"
#include "Game.h"
#include "Components.h"
#include "Prefabs.h"
#include "TrajectoryMath.h"
#include "LevelGenerator.h"

#include <functional>
#include <random>
#include <iostream>

#include <SFML/Graphics/RectangleShape.hpp>

PlayingState::PlayingState(Game* game, int level) :
	m_Game(game),
	m_CurrGameLevel(level)
{ }

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
	world.RegisterComponent<Enemy>();
	world.RegisterComponent<Button>();

	//Register Systems
	m_RenderSystem = world.RegisterSystem<RenderSystem>();
	m_InputSystem = world.RegisterSystem<InputSystem>();
	m_ColliderSystem = world.RegisterSystem<ColliderSystem>();
	m_PhysicsSystem = world.RegisterSystem<PhysicsSystem>();
	m_LifetimeSystem = world.RegisterSystem<LifetimeSystem>();
	m_EnemySystem = world.RegisterSystem<EnemySystem>();
	m_UISystem = world.RegisterSystem<UISystem>();

	//Set Systems World
	m_RenderSystem->SetWorld(&world);
	m_InputSystem->SetWorld(&world);
	m_ColliderSystem->SetWorld(&world);
	m_PhysicsSystem->SetWorld(&world);
	m_LifetimeSystem->SetWorld(&world);
	m_EnemySystem->SetWorld(&world);
	m_UISystem->SetWorld(&world);

	m_EnemySystem->SetTextureManager(&m_Game->GetTextureManager()); //Set Texture Manager referenced by enemySystem
	m_EnemySystem->SetSoundManager(&m_Game->GetSoundManager());
	m_ColliderSystem->SetTextureManager(&m_Game->GetTextureManager()); //Set Texture Manager referenced by colliderSystem
	m_ColliderSystem->SetSoundManager(&m_Game->GetSoundManager());

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

	Signature enemySignature;
	enemySignature.set(world.GetComponentID<Enemy>());
	world.SetSystemSignature<EnemySystem>(enemySignature);

	Signature uiSignature;
	uiSignature.set(world.GetComponentID<Transform>());
	uiSignature.set(world.GetComponentID<Button>());
	world.SetSystemSignature<UISystem>(uiSignature);

	m_Game->GetTextureManager().InitGameAnimationData();

	LevelGenerator levelGen(m_Game);
	levelGen.LoadLevel(m_CurrGameLevel);

	//Delay input enabling (until sliding door fully opens)
#ifdef _DEBUG
	SetInputDisabled(false);
#else
	world.AddComponentToEntity<Lifetime>(world.CreateEntity(), { 2,0,[&](Entity) {SetInputDisabled(false); } });
#endif
}

void PlayingState::Exit() {
	//std::println("[PlayingState] Exiting...");

	m_RenderSystem.reset();
	m_InputSystem.reset();
	m_PhysicsSystem.reset();
	m_ColliderSystem.reset();
	m_LifetimeSystem.reset();
	m_EnemySystem.reset();
	m_UISystem.reset();

	//m_Game->m_SoundManager.StopAllSound();
	m_Game->GetWorld().ResetManagers();
}

void PlayingState::Update(sf::RenderWindow& renderWindow, const float& deltaTime) {

	//Check for next stage / end stage logic updates
	if (m_GameOverCnt > 0) m_GameOverCnt -= deltaTime;
	if (m_NextStageCnt > 0) m_NextStageCnt -= deltaTime;

	if (std::abs(m_GameOverCnt) < 0.01 && !m_GamePaused) {
		m_GamePaused = true;
		EndGame(false);
	}
	else if (std::abs(m_NextStageCnt) < 0.01) {
		NextStage();
	}
	
	//Systems Update
	if (!m_GamePaused) {
		m_InputSystem->Update(renderWindow, deltaTime);
		m_PhysicsSystem->Update(deltaTime);
		m_LifetimeSystem->Update(deltaTime);
		m_ColliderSystem->Update();
		m_EnemySystem->Update(deltaTime);

		//Update variables
		if (shurikenCD > 0) {
			shurikenCD -= deltaTime;
			if (shurikenCD <= 0) m_InputSystem->SetKeyboardDisabled(false);
		}

		if (m_GameOverCnt == -1) UpdatePlayerState(renderWindow);
	}
	else m_UISystem->Update(renderWindow.mapPixelToCoords(sf::Mouse::getPosition(renderWindow)));

	if (m_EnemySystem->getEnemyCount() == 0 && m_NextStageCnt == -1) m_NextStageCnt = 2.5f;
}

void PlayingState::Render(sf::RenderWindow& renderWindow, float deltaTime) {
	if (m_GamePaused) deltaTime = 0;
	m_RenderSystem->Update(renderWindow, deltaTime);
}

void PlayingState::HandleEvents(const sf::Event& event) {
	if (event.is< sf::Event::FocusLost >()) {
		m_InputSystem->SetWindowFocused(false);
	}
	else if (event.is < sf::Event::FocusGained>()) {
		m_InputSystem->SetWindowFocused(true);
	}
	if (m_GamePaused) {
		sf::RenderWindow& gameWindow = m_Game->GetWindow();
		m_UISystem->HandleEvents(event, gameWindow.mapPixelToCoords(sf::Mouse::getPosition(gameWindow)));
	}
		
}

void PlayingState::UpdatePlayerState(sf::RenderWindow& renderWindow) {
	PlayerInputIntent input = m_InputSystem->GetIntent();

	World& world = m_Game->GetWorld();

#ifdef _DEBUG
	if (input.reset) {
		m_Game->m_StateManager.EnqueueStateChange(CreateScope<PlayingState>(m_Game));
	}
#endif

	if (input.pause) {
		m_GamePaused = true;
		PauseGame();
		return;
	}
		

	Entity playerEnt = world.GetAllComponentsOfType<Player>()[0].first; //Get player entity
	Player& playerComp = world.GetComponent<Player>(playerEnt);

	if (playerComp.health <= 0) {
		EnqueueGameOver();
		return;
	}

	Transform& playerTransform = world.GetComponent<Transform>(playerEnt);
	Renderable& playerRenderable = world.GetComponent<Renderable>(playerEnt);
	Physics& playerPhysics = world.GetComponent<Physics>(playerEnt);
	AnimationData& playerAnimation = world.GetComponent<AnimationData>(playerEnt);
	Collider& playerCollider = world.GetComponent<Collider>(playerEnt);

	if ((!input.walkLeft && !input.walkRight) || input.isAiming) playerPhysics.velocity.x = 0;

	if (input.dropDown && playerCollider.canPhaseThroughPlatform) {
		playerCollider.phaseThroughPlatform = true;
	}
	else {
		playerCollider.phaseThroughPlatform = false;
	}

	if (playerPhysics.isGrounded) {
		if (input.projectileReleased && shurikenCD <= 0.0f) {
			ThrowShuriken(renderWindow, m_Game->GetWindow().mapPixelToCoords(input.mousePos));
			playerComp.playerState = PlayerState::Throwing;
		}
		else if (input.isAiming) {
			if (!input.readyToShoot) {
				playerComp.playerState = PlayerState::Aiming;
				if (m_Game->GetTextureManager().ChangeEntitySprite("Player_Aiming", playerRenderable, playerAnimation)) {
					playerAnimation.animationEvents.insert({ 2, [&]() {m_Game->GetSoundManager().PlaySound("Metal_Clank"); }});
				}
			}
			else {
				playerComp.playerState = PlayerState::ReadyToShoot;
				if (m_Game->GetTextureManager().ChangeEntitySprite("Player_Aim_Ready", playerRenderable, playerAnimation)) {
					m_Game->GetSoundManager().PlaySound("Ready_Tinkle");
				}
			}
			sf::FloatRect playerBox{ {world.GetComponent<Transform>(playerEnt).position},
						 {world.GetComponent<Renderable>(playerEnt).size} };
			sf::Vector2f shurikenDir = (m_Game->GetWindow().mapPixelToCoords(input.mousePos) - playerBox.getCenter()).normalized();

			sf::Vector2f trajectoryEnd;
			float shurikenRadius = 15.f; //Hardcode for now
			for (auto& [start, end] : ComputeTrajectory(world, playerBox.getCenter(), shurikenDir, shurikenRadius, 2)) {
				Entity assistLine = world.CreateEntity();

				Transform trans;
				trans.position = start;
				trans.rotation = (end - start).angle().asDegrees();
				world.AddComponentToEntity<Transform>(assistLine, trans);

				Renderable render;
				render.size = { (end - start).length() , 2 };

				render.texture = &m_Game->GetTextureManager().Load("Trajectory_Preview");
				world.AddComponentToEntity<Renderable>(assistLine, std::move(render));

				world.AddComponentToEntity<Lifetime>(assistLine, Lifetime{ 0.001f }); //Lasts 1 frame only
				trajectoryEnd = end;
			}

			Entity preview = world.CreateEntity();
			// Adjust shuriken preview to be centered at line's end
			world.AddComponentToEntity<Transform>(preview, Transform{ { trajectoryEnd.x - shurikenRadius, trajectoryEnd.y - shurikenRadius } });
			world.AddComponentToEntity<Renderable>(preview, Renderable{ {30,30}, &m_Game->GetTextureManager().Load("Shuriken_Preview"), nullptr, RenderLayer::UI });
			world.AddComponentToEntity<Lifetime>(preview, Lifetime{ 0.01f });

			std::abs(shurikenDir.angle().asDegrees()) <= 90 ? playerRenderable.flipX = false : playerRenderable.flipX = true;

		}
		else if (shurikenCD <= 0) {
			if (input.jump) {
				playerPhysics.velocity.y -= playerComp.jumpForce;

				m_Game->GetTextureManager().ChangeEntitySprite("Player_Jump_Sprite", playerRenderable, playerAnimation);
			}
			else if (input.walkLeft || input.walkRight) {
				m_Game->GetTextureManager().ChangeEntitySprite("Player_Sprint_Sprite", playerRenderable, playerAnimation);
				playerComp.playerState = PlayerState::Walking;
				if (input.walkLeft) {
					playerPhysics.velocity.x = -300;
					playerRenderable.flipX = true;
				}
				else {
					playerPhysics.velocity.x = 300;
					playerRenderable.flipX = false;
				}
			}
			else {
				playerPhysics.velocity.x = 0.f;
				playerComp.playerState = PlayerState::Idle;
				m_Game->GetTextureManager().ChangeEntitySprite("Player_Idle_Sprite", playerRenderable, playerAnimation);
			}
		}
	}
	else {
		m_Game->GetTextureManager().ChangeEntitySprite("Player_Jump_Sprite", playerRenderable, playerAnimation);
		playerComp.playerState = PlayerState::Airborne;
		playerPhysics.velocity.x = 0;
		m_InputSystem->ResetMouseHeldDuration();
	}
}

void PlayingState::ThrowShuriken(sf::RenderWindow& renderWindow, sf::Vector2f mousePos){
	Entity playerEnt = m_Game->GetWorld().GetAllComponentsOfType<Player>()[0].first;
	World& world = m_Game->GetWorld();
	sf::FloatRect playerBox{ {world.GetComponent<Transform>(playerEnt).position},
						 {world.GetComponent<Renderable>(playerEnt).size} };

	PrefabContext prefabCtx = { world, &m_Game->GetTextureManager(), &m_Game->GetSoundManager() };

	Entity shuriken = PrefabGen::Shuriken(prefabCtx);

	sf::CircleShape shurikenCollider = std::get<sf::CircleShape>(world.GetComponent<Collider>(shuriken).entityColliders[0].first);
	world.GetComponent<Transform>(shuriken) = { playerBox.getCenter() - sf::Vector2f{shurikenCollider.getRadius(), shurikenCollider.getRadius()} };

	sf::Vector2f shurikenDir = mousePos - playerBox.getCenter();
	sf::Vector2f normalizedDir = shurikenDir.normalized();

	world.GetComponent<Physics>(shuriken) = { {normalizedDir.x * 1000, normalizedDir.y * 1000 } };

	world.GetComponent<Collider>(shuriken).OnCollision = [&](Entity shuriken) {
		m_Game->GetSoundManager().PlaySound("Shuriken_Impact");
		m_Game->GetSoundManager().PlaySound("Wooden_Impact");
		};

	Renderable& playerRenderable = world.GetComponent<Renderable>(playerEnt);
	AnimationData& playerAnimation = world.GetComponent<AnimationData>(playerEnt);

	m_Game->GetTextureManager().ChangeEntitySprite("Player_Throw_Sprite", playerRenderable, playerAnimation);

	//Refresh cooldown
	shurikenCD += 0.45f;
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

	PrefabContext prefabCtx = { world, &m_Game->GetTextureManager() };
	Entity target = PrefabGen::Target(prefabCtx);
	world.GetComponent<Transform>(target) = { {spawnX, spawnY} };

	std::function<void(Entity ent)> onTargetDestroyed = [this](Entity target) {
		World& world = m_Game->GetWorld();
		Entity destroyEffect = world.CreateEntity();
		world.AddComponentToEntity<Transform>(destroyEffect, world.GetComponent<Transform>(target));
		sf::Vector2f& targetSize = world.GetComponent<Renderable>(target).size;
		world.AddComponentToEntity<Renderable>(destroyEffect, Renderable{ {targetSize.x, targetSize.y}, &m_Game->GetTextureManager().Load("Explosion"), nullptr, RenderLayer::GameObject1 });
		world.AddComponentToEntity<AnimationData>(destroyEffect, AnimationData{ {5,2}, 10, 0.05f });
		world.AddComponentToEntity<Lifetime>(destroyEffect, Lifetime{ 0.45f ,0 });
		m_Game->GetSoundManager().PlaySound("Vine_Boom");
		SpawnTarget();
		};
	world.GetComponent<Lifetime>(target).OnDestroyedFunction = onTargetDestroyed;

	Entity spawnSmoke = world.CreateEntity();
	world.AddComponentToEntity<Transform>(spawnSmoke, Transform{ {spawnX, spawnY} });
	world.AddComponentToEntity<Renderable>(spawnSmoke, Renderable{ { 60, 100 }, &m_Game->GetTextureManager().Load("Smoke"), nullptr, RenderLayer::GameObject1});
	world.AddComponentToEntity<AnimationData>(spawnSmoke, AnimationData{ {6,1}, 6, 0.1f });
	world.AddComponentToEntity<Lifetime>(spawnSmoke, Lifetime{ 0.5f,0 });
}

void PlayingState::EnqueueGameOver() {
	World& world = m_Game->GetWorld();
	Entity playerEnt = world.GetAllComponentsOfType<Player>()[0].first; //Get player entity
	Transform& playerTrans = world.GetComponent<Transform>(playerEnt);
	Renderable& playerRend = world.GetComponent<Renderable>(playerEnt);
	AnimationData& playerAnim = world.GetComponent<AnimationData>(playerEnt);
	Physics& playerPhys = world.GetComponent<Physics>(playerEnt);
	Collider& playerCollider = world.GetComponent<Collider>(playerEnt);
	playerTrans.scale.x = 1.2;
	playerPhys.velocity.x = 0;
	m_Game->GetTextureManager().ChangeEntitySprite("Player_Death_Sprite", playerRend, playerAnim);
	m_GameOverCnt = 2.5f;
}

void PlayingState::PauseGame() {
	World& world = m_Game->GetWorld();

	Entity darkOverlay = world.CreateEntity();
	world.AddComponentToEntity<Transform>(darkOverlay, Transform{});
	world.AddComponentToEntity<Renderable>(darkOverlay, Renderable{ {1280, 720}, nullptr, nullptr, RenderLayer::UI, true, false, sf::Color{0,0,0, 180} });
	world.AddComponentToEntity<Button>(darkOverlay, Button{});

	Entity contButton = world.CreateEntity();
	world.AddComponentToEntity<Transform>(contButton, Transform{ { 400, 350 } });
	world.AddComponentToEntity<Renderable>(contButton, Renderable{ { 420, 100 }, &m_Game->GetTextureManager().Load("UI/Button_Continue"), nullptr, RenderLayer::UI });
	world.AddComponentToEntity<Button>(contButton, Button{ {420, 100}, [&]() {
		m_Game->GetSoundManager().PlaySound("Button_Click");
		m_GamePaused = false;
		for (auto& [ent, buttonComp] : world.GetAllComponentsOfType<Button>()) {
			world.DestroyEntity(ent);
		}
		} });

	Entity returnButton = world.CreateEntity();
	world.AddComponentToEntity<Transform>(returnButton, Transform{ { 400, 500 } });
	world.AddComponentToEntity<Renderable>(returnButton, Renderable{ { 420, 100 }, &m_Game->GetTextureManager().Load("UI/Button_Return_MainMenu"), nullptr, RenderLayer::UI });
	world.AddComponentToEntity<Button>(returnButton, Button{ {420, 100}, [&]() {
		m_Game->GetSoundManager().PlaySound("Button_Click");
		m_Game->m_StateManager.EnqueueStateChange(CreateScope<MainMenuState>(this->m_Game));
	} });

	//Volume Control
	Entity volumeBar = world.CreateEntity();
	world.AddComponentToEntity<Transform>(volumeBar, Transform{ {950, 415} });
	world.AddComponentToEntity<Renderable>(volumeBar, Renderable{ { 300, 55 }, &m_Game->GetTextureManager().Load("UI/Volume_Bar"), nullptr, RenderLayer::UI, false });
	world.AddComponentToEntity<Button>(volumeBar, Button{});

	Entity volumeSlider = world.CreateEntity();
	float sliderX = 975 + ((float)GlobalVolumeLevel / DefaultVolumeSetting * 225);
	world.AddComponentToEntity<Transform>(volumeSlider, Transform{ {sliderX, 428}});
	world.AddComponentToEntity<Renderable>(volumeSlider, Renderable{ { 25, 25 }, &m_Game->GetTextureManager().Load("UI/Volume_Slider"), nullptr, RenderLayer::UI, false });
	world.AddComponentToEntity<Button>(volumeSlider, Button{ {65, 60}, []() {}, [&, volumeSlider](sf::Vector2f mousePos) {
		Renderable& sliderRend = m_Game->GetWorld().GetComponent<Renderable>(volumeSlider);
		if (!sliderRend.visible) return;
		Transform& sliderTrans = m_Game->GetWorld().GetComponent<Transform>(volumeSlider);
		sliderTrans.position.x = std::clamp((int)mousePos.x, 975, 1200);
		GlobalVolumeLevel = (sliderTrans.position.x - 975) / (1200 - 975) * DefaultVolumeSetting;
		m_Game->GetSoundManager().SetMusicVolume(GlobalVolumeLevel);
	}
		});

	Entity volumeButton = world.CreateEntity();
	world.AddComponentToEntity<Transform>(volumeButton, Transform{ {870, 415} });
	world.AddComponentToEntity<Renderable>(volumeButton, Renderable{ { 65, 60 }, &m_Game->GetTextureManager().Load("UI/Volume_Button"), nullptr, RenderLayer::UI });
	world.AddComponentToEntity<Button>(volumeButton, Button{ {65, 60}, [this, volumeBar, volumeSlider]() {
		m_Game->GetSoundManager().PlaySound("Button_Click");
		Renderable& volumeBarRend = m_Game->GetWorld().GetComponent<Renderable>(volumeBar);
		Renderable& volumeSliderRend = m_Game->GetWorld().GetComponent<Renderable>(volumeSlider);
		volumeBarRend.visible = !volumeBarRend.visible;
		volumeSliderRend.visible = !volumeSliderRend.visible;
	} });
}

void PlayingState::EndGame(bool gameWon) {
	World& world = m_Game->GetWorld();

	Entity darkOverlay = world.CreateEntity();
	world.AddComponentToEntity<Transform>(darkOverlay, Transform{});
	world.AddComponentToEntity<Renderable>(darkOverlay, Renderable{ {1280, 720}, nullptr, nullptr, RenderLayer::UI, true, false, sf::Color{0,0,0, 180} });

	Entity textOverlay = world.CreateEntity();
	world.AddComponentToEntity<Transform>(textOverlay, Transform{ {320, 100} });
	world.AddComponentToEntity<Renderable>(textOverlay, Renderable{ {600, 150}, nullptr, nullptr, RenderLayer::UI });
	if (gameWon) world.GetComponent<Renderable>(textOverlay).texture = &m_Game->GetTextureManager().Load("UI/Text_Congratulations");
	else world.GetComponent<Renderable>(textOverlay).texture = &m_Game->GetTextureManager().Load("UI/Text_You_Died");

	Entity restartButton = world.CreateEntity();
	world.AddComponentToEntity<Transform>(restartButton, Transform{ { 400, 350 } });
	world.AddComponentToEntity<Renderable>(restartButton, Renderable{ { 420, 100 }, nullptr, nullptr, RenderLayer::UI });
	if (gameWon) world.GetComponent<Renderable>(restartButton).texture = &m_Game->GetTextureManager().Load("UI/Button_Play_Again");
	else world.GetComponent<Renderable>(restartButton).texture = &m_Game->GetTextureManager().Load("UI/Button_Restart");

	world.AddComponentToEntity<Button>(restartButton, Button{ {420, 100}, [&, gameWon]() {
		m_Game->GetSoundManager().PlaySound("Button_Click");
		if (gameWon) m_Game->m_StateManager.EnqueueStateChange(CreateScope<PlayingState>(this->m_Game));
		else m_Game->m_StateManager.EnqueueStateChange(CreateScope<PlayingState>(this->m_Game, m_CurrGameLevel));
		} });

	Entity returnButton = world.CreateEntity();
	world.AddComponentToEntity<Transform>(returnButton, Transform{ { 400, 500 } });
	world.AddComponentToEntity<Renderable>(returnButton, Renderable{ { 420, 100 }, &m_Game->GetTextureManager().Load("UI/Button_Return_MainMenu"), nullptr, RenderLayer::UI });
	world.AddComponentToEntity<Button>(returnButton, Button{ {420, 100}, [&]() {
		m_Game->GetSoundManager().PlaySound("Button_Click");
		m_Game->m_StateManager.EnqueueStateChange(CreateScope<MainMenuState>(this->m_Game));
	} });

	if (gameWon) m_Game->GetSoundManager().PlaySound("Victory");
}

void PlayingState::NextStage() {

	if (!m_SwitchingStage) {
		m_SwitchingStage = true;

		if (m_CurrGameLevel == m_TotalStages) {
			m_GamePaused = true;
			EndGame(true);
			return;
		}

		m_CurrGameLevel++;

		PrefabContext prefabCtx = { m_Game->GetWorld(), &m_Game->GetTextureManager()};
		
		Entity leftDoor = PrefabGen::SlidingDoor(prefabCtx, { -640, 0 }, false, true);
		prefabCtx.world.GetComponent<Lifetime>(leftDoor).OnDestroyedFunction = [&, prefabCtx](Entity) {
			m_Game->m_StateManager.EnqueueStateChange(CreateScope<PlayingState>(this->m_Game, m_CurrGameLevel));
			PrefabGen::StaticDoor(prefabCtx, {0,0}, false);
		};

		Entity rightDoor = PrefabGen::SlidingDoor(prefabCtx, { 1280, 0 }, true, false);
		m_Game->GetWorld().GetComponent<Lifetime>(rightDoor).OnDestroyedFunction = [&, prefabCtx](Entity) {
			PrefabGen::StaticDoor(prefabCtx, { 640,0 }, true);
			};

		m_Game->GetSoundManager().PlaySound("Sliding_Door");
		SetInputDisabled(true);
	}
}

void PlayingState::SetInputDisabled(bool disabled) {
	m_InputSystem->SetMouseDisabled(disabled);
	m_InputSystem->SetKeyboardDisabled(disabled);
}