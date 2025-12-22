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
	world.AddComponentToEntity<Renderable>(background, {{ 1280, 720 }, RenderLayer::Background, &m_Game->m_TextureManager.Load("Dojo_TitleScreen"), true});

	//Title
	Entity title = world.CreateEntity();
	world.AddComponentToEntity<Transform>(title, { {430.0f,60.0f} });
	world.AddComponentToEntity<Renderable>(title, { { 420, 260 }, RenderLayer::UI, &m_Game->m_TextureManager.Load("ShurikenTactics_Title"), true });

	// --- Buttons ---
	Entity startButton = world.CreateEntity();
	world.AddComponentToEntity<Transform>(startButton, { {430.0f,400.0f} });
	world.AddComponentToEntity<Renderable>(startButton, { { 420, 100 }, RenderLayer::UI, &m_Game->m_TextureManager.Load("Button_StartGame"), true });
	world.AddComponentToEntity<Button>(startButton, { {420, 100}, [this]() {
		m_Game->GetSoundManager().PlaySound("Button_Click");
		m_Game->m_StateManager.ChangeState(CreateScope<PlayingState>(this->m_Game));
	}});

	Entity quitButton = world.CreateEntity();
	world.AddComponentToEntity<Transform>(quitButton, { {430.0f, 530.0f} });
	world.AddComponentToEntity<Renderable>(quitButton, { { 420, 100 }, RenderLayer::UI, &m_Game->m_TextureManager.Load("Button_QuitGame"), true });
	world.AddComponentToEntity<Button>(quitButton, { {420, 100}, [this]() {
		m_Game->GetSoundManager().PlaySound("Button_Click");
		m_Game->GetWindow().close();
	} });

	//Volume Control
	Entity volumeBar = world.CreateEntity();
	world.AddComponentToEntity<Transform>(volumeBar, { {950, 415} });
	world.AddComponentToEntity<Renderable>(volumeBar, { { 300, 55 }, RenderLayer::UI, &m_Game->m_TextureManager.Load("UI/Volume_Bar"), false });

	Entity volumeSlider = world.CreateEntity();
	world.AddComponentToEntity<Transform>(volumeSlider, { {1200, 428} });
	world.AddComponentToEntity<Renderable>(volumeSlider, { { 25, 25 }, RenderLayer::UI, &m_Game->m_TextureManager.Load("UI/Volume_Slider"), false });
	world.AddComponentToEntity<Button>(volumeSlider, { {65, 60}, []() {}, [&, volumeSlider](sf::Vector2f mousePos) {
		Renderable& sliderRend = m_Game->GetWorld().GetComponent<Renderable>(volumeSlider);
		if (!sliderRend.visible) return;
		Transform& sliderTrans = m_Game->GetWorld().GetComponent<Transform>(volumeSlider);
		sliderTrans.position.x = std::clamp((int)mousePos.x, 975, 1200);
		GlobalVolumeLevel = (sliderTrans.position.x - 975) / (1200 - 975) * 20;
		m_Game->GetSoundManager().SetMusicVolume(GlobalVolumeLevel);
	}
	});

	Entity volumeButton = world.CreateEntity();
	world.AddComponentToEntity<Transform>(volumeButton, { {870, 415} });
	world.AddComponentToEntity<Renderable>(volumeButton, { { 65, 60 }, RenderLayer::UI, &m_Game->m_TextureManager.Load("UI/Volume_Button"), true });
	world.AddComponentToEntity<Button>(volumeButton, { {65, 60}, [this, volumeBar, volumeSlider]() {
		m_Game->GetSoundManager().PlaySound("Button_Click");
		Renderable& volumeBarRend = m_Game->GetWorld().GetComponent<Renderable>(volumeBar);
		Renderable& volumeSliderRend = m_Game->GetWorld().GetComponent<Renderable>(volumeSlider);
		volumeBarRend.visible = !volumeBarRend.visible;
		volumeSliderRend.visible = !volumeSliderRend.visible;
	} });

	//BGM
	m_Game->m_SoundManager.PlayMusic("Afternoon_In_Konoha");
}

void MainMenuState::Exit() {
	std::println("[MainMenuState] Exiting...");
	m_RenderSystem.reset();
	m_UISystem.reset();
	//m_Game->m_SoundManager.StopAll();
	m_Game->GetWorld().ResetManagers();
}

void MainMenuState::Update(sf::RenderWindow& renderWindow, const float& deltaTime) {
	sf::RenderWindow& gameWindow = m_Game->GetWindow();
	m_UISystem->Update(gameWindow.mapPixelToCoords(sf::Mouse::getPosition(gameWindow)));
}

void MainMenuState::Render(sf::RenderWindow& renderWindow, const float& deltaTime) {
	m_RenderSystem->Update(renderWindow, deltaTime);
}

void MainMenuState::HandleEvents(const sf::Event& event) {
	sf::RenderWindow& gameWindow = m_Game->GetWindow();
	m_UISystem->HandleEvents(event, gameWindow.mapPixelToCoords(sf::Mouse::getPosition(gameWindow)));
}