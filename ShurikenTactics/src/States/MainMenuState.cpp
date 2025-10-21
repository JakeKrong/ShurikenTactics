#include "MainMenuState.h"
#include "PlayingState.h"
#include "Game.h"
#include "Components.h"
#include <print>

MainMenuState::MainMenuState(Game* game) :
	m_Game (game) { }

void MainMenuState::Enter() {  //Initialise Main Menu
	World& world = m_Game->GetWorld();
	//Register Components
	world.RegisterComponent<Transform>();
	world.RegisterComponent<Renderable>();
	world.RegisterComponent<Button>();

	//Register Systems
	m_RenderSystem = world.RegisterSystem<RenderSystem>();
	m_UISystem = world.RegisterSystem<UISystem>();

	m_RenderSystem->SetWorld(&world);
	m_UISystem->SetWorld(&world);

	//Set System Signature
	Signature renderSignature;
	renderSignature.set(world.GetComponentID<Transform>());
	renderSignature.set(world.GetComponentID<Renderable>());
	world.SetSystemSignature<RenderSystem>(renderSignature);

	Signature uiSignature;
	uiSignature.set(world.GetComponentID<Transform>());
	uiSignature.set(world.GetComponentID<Button>());
	world.SetSystemSignature<UISystem>(uiSignature);

	// *** Set Entites *** //
	//Background
	Entity background = world.CreateEntity();
	world.AddComponentToEntity<Transform>(background, { {0.0f,0.0f} });
	world.AddComponentToEntity<Renderable>(background, {{ 1280, 720 }, true, &m_Game->m_TextureManager.Load("Dojo_TitleScreen.png"), false});

	//Title
	Entity title = world.CreateEntity();
	world.AddComponentToEntity<Transform>(title, { {430.0f,60.0f} });
	world.AddComponentToEntity<Renderable>(title, { { 420, 260 }, true, &m_Game->m_TextureManager.Load("ShurikenTactics_Title.png"), false });

	//Buttons
	Entity startButton = world.CreateEntity();
	world.AddComponentToEntity<Transform>(startButton, { {430.0f,400.0f} });
	world.AddComponentToEntity<Renderable>(startButton, { { 420, 100 }, true, &m_Game->m_TextureManager.Load("Button_StartGame.png"), false });
	world.AddComponentToEntity<Button>(startButton, { {420, 100}, [this]() {
		this->m_Game->m_StateManager.ChangeState(CreateScope<PlayingState>(this->m_Game));
	}});

	Entity quitButton = world.CreateEntity();
	world.AddComponentToEntity<Transform>(quitButton, { {430.0f, 530.0f} });
	world.AddComponentToEntity<Renderable>(quitButton, { { 420, 100 }, true, &m_Game->m_TextureManager.Load("Button_QuitGame.png"), false });
	world.AddComponentToEntity<Button>(quitButton, { {420, 100}, [this]() {
		this->m_Game->GetWindow().close();
	} });

	//BGM
	sf::Sound& bgm = m_Game->m_SoundManager.Load("Afternoon_In_Konoha.mp3");
	bgm.setLooping(true);
	bgm.setVolume(10);
	bgm.play();
}

void MainMenuState::Exit() {
	std::println("[MainMenuState] Exiting...");
	m_RenderSystem.reset();
	m_UISystem.reset();
	//m_Game->m_SoundManager.StopAll();
	m_Game->GetWorld().ResetManagers();
}

void MainMenuState::Update(sf::RenderWindow& renderWindow, const float& deltaTime) {
	
}

void MainMenuState::Render(sf::RenderWindow& renderWindow, const float& deltaTime) {
	m_RenderSystem->Update(renderWindow, deltaTime);
}

void MainMenuState::HandleEvents(const sf::Event& event) {
	sf::RenderWindow& gameWindow = m_Game->GetWindow();
	m_UISystem->HandleEvents(event, gameWindow.mapPixelToCoords(sf::Mouse::getPosition(gameWindow)));
}