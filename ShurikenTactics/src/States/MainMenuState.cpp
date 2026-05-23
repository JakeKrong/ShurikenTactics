#include "MainMenuState.h"
#include "PlayingState.h"
#include "Game.h"
#include "Components.h"
#include "Prefabs.h"

#include <print>
#include <iostream>

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
	world.AddComponentToEntity<Transform>(background, Transform{ {0.0f,0.0f} });
	world.AddComponentToEntity<Renderable>(background, Renderable{{ 1280, 720 }, &m_Game->GetTextureManager().Load("Dojo_TitleScreen"), nullptr, RenderLayer::Background });

	//Title
	Entity title = world.CreateEntity();
	world.AddComponentToEntity<Transform>(title, Transform{ {430,60} });
	world.AddComponentToEntity<Renderable>(title, Renderable{ { 420, 260 }, &m_Game->GetTextureManager().Load("UI/ShurikenTactics_Title"), nullptr, RenderLayer::UI });

	// --- Buttons ---
	Entity startButton = world.CreateEntity();
	world.AddComponentToEntity<Transform>(startButton, Transform{ {430,400} });
	world.AddComponentToEntity<Renderable>(startButton, Renderable{ { 420, 100 }, &m_Game->GetTextureManager().Load("UI/Button_StartGame"), nullptr, RenderLayer::UI });
	world.AddComponentToEntity<Button>(startButton, Button{ {420, 100}, [this]() {
		m_Game->GetSoundManager().PlaySound("Button_Click");
		m_Game->m_StateManager.EnqueueStateChange(CreateScope<PlayingState>(m_Game));
	}});

	Entity quitButton = world.CreateEntity();
	world.AddComponentToEntity<Transform>(quitButton, Transform{ {430.0f, 530.0f} });
	world.AddComponentToEntity<Renderable>(quitButton, Renderable{ { 420, 100 }, &m_Game->GetTextureManager().Load("UI/Button_QuitGame"), nullptr, RenderLayer::UI });
	world.AddComponentToEntity<Button>(quitButton, Button{ {420, 100}, [this]() {
		m_Game->GetSoundManager().PlaySound("Button_Click");
		m_Game->GetWindow().close();
	} });

	Entity tutorialButton = world.CreateEntity();
	world.AddComponentToEntity<Transform>(tutorialButton, Transform{ {350, 415} });
	world.AddComponentToEntity<Renderable>(tutorialButton, Renderable{ { 65, 60 }, &m_Game->GetTextureManager().Load("UI/Question_Button"), nullptr, RenderLayer::UI });
	world.AddComponentToEntity<Button>(tutorialButton, Button{ {65, 60}, [this]() {
		EnableButtons(false);
		ShowTutorial(); 
		m_Game->GetSoundManager().PlaySound("Button_Click");
		} });

	//Volume Control
	Entity volumeBar = world.CreateEntity();
	world.AddComponentToEntity<Transform>(volumeBar, Transform{ {950, 415} });
	world.AddComponentToEntity<Renderable>(volumeBar, Renderable{ { 300, 55 }, &m_Game->GetTextureManager().Load("UI/Volume_Bar"), nullptr, RenderLayer::UI, false });

	Entity volumeSlider = world.CreateEntity();
	float sliderX = 975 + ((float)GlobalVolumeLevel / DefaultVolumeSetting * 225);
	world.AddComponentToEntity<Transform>(volumeSlider, Transform{ {sliderX, 428} });
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

	//BGM
	//m_Game->m_SoundManager.PlayMusic("Afternoon_In_Konoha");
}

void MainMenuState::Exit() {
	//std::println("[MainMenuState] Exiting...");
	m_RenderSystem.reset();
	m_UISystem.reset();
	//m_Game->m_SoundManager.StopAll();
	m_Game->GetWorld().ResetManagers();
}

void MainMenuState::Update(sf::RenderWindow& renderWindow, const float& deltaTime) {
	sf::RenderWindow& gameWindow = m_Game->GetWindow();
	m_UISystem->Update(gameWindow.mapPixelToCoords(sf::Mouse::getPosition(gameWindow)));
}

void MainMenuState::Render(sf::RenderWindow& renderWindow, float deltaTime) {
	m_RenderSystem->Update(renderWindow, deltaTime);
}

void MainMenuState::HandleEvents(const sf::Event& event) {
	sf::RenderWindow& gameWindow = m_Game->GetWindow();
	m_UISystem->HandleEvents(event, gameWindow.mapPixelToCoords(sf::Mouse::getPosition(gameWindow)));
}

void MainMenuState::ShowTutorial() {
	World& world = m_Game->GetWorld();
	int totalTutorialPages = 5;

	Entity darkOverlay = world.CreateEntity();
	world.AddComponentToEntity<Transform>(darkOverlay, Transform{});
	world.AddComponentToEntity<Renderable>(darkOverlay, Renderable{ {1280, 720}, nullptr, nullptr, RenderLayer::UI, true, false, sf::Color{0,0,0,220} });

	Entity tutorialImg = world.CreateEntity();
	world.AddComponentToEntity<Transform>(tutorialImg, Transform{ {240, 20} });
	world.AddComponentToEntity<Renderable>(tutorialImg, Renderable{ {800, 450}, &m_Game->GetTextureManager().Load("Tutorial/1"), nullptr, RenderLayer::UI });

	PrefabContext prefabCtx = { world, &m_Game->GetTextureManager(), &m_Game->GetSoundManager(), &m_Game->GetFontManager()};

	Entity tutorialText = PrefabGen::Text(prefabCtx, { 200, 500 }, PrefabGen::TutorialText(1), 20, sf::Color::White);
	Entity pageInd = PrefabGen::Text(prefabCtx, { 618, 650 }, "1 / " + std::to_string(totalTutorialPages), 20, sf::Color::White);

	Entity prevPageButton = world.CreateEntity();
	world.AddComponentToEntity<Transform>(prevPageButton, Transform{ {100, 500} });
	world.AddComponentToEntity<Renderable>(prevPageButton, Renderable{ {30, 30}, &m_Game->GetTextureManager().Load("UI/Button_Next"), nullptr, RenderLayer::UI });
	world.AddComponentToEntity<Button>(prevPageButton, Button{ {30, 30}, [&, pageInd, tutorialImg, tutorialText, totalTutorialPages]() {
		m_Game->GetSoundManager().PlaySound("Button_Click");
		int pageNumber = world.GetComponent<Renderable>(pageInd).text->getString()[0] - '0';

		pageNumber--;
		if (pageNumber == 0) pageNumber = totalTutorialPages;

		world.GetComponent<Renderable>(pageInd).text->setString(std::to_string(pageNumber) + " / " + std::to_string(totalTutorialPages));
		world.GetComponent<Renderable>(tutorialImg).texture = &m_Game->GetTextureManager().Load("Tutorial/" + std::to_string(pageNumber));
		world.GetComponent<Renderable>(tutorialText).text->setString(PrefabGen::TutorialText(pageNumber));

		} });

	Entity nextPageButton = world.CreateEntity();
	world.AddComponentToEntity<Transform>(nextPageButton, Transform{ {1150, 500} });
	world.AddComponentToEntity<Renderable>(nextPageButton, Renderable{ {30, 30}, &m_Game->GetTextureManager().Load("UI/Button_Next"), nullptr, RenderLayer::UI });
	world.AddComponentToEntity<Button>(nextPageButton, Button{ {30, 30}, [&, pageInd, tutorialImg, tutorialText, totalTutorialPages]() {
		m_Game->GetSoundManager().PlaySound("Button_Click");
		int pageNumber = world.GetComponent<Renderable>(pageInd).text->getString()[0] - '0';

		pageNumber++;
		if (pageNumber > totalTutorialPages) pageNumber = 1;

		world.GetComponent<Renderable>(pageInd).text->setString(std::to_string(pageNumber) + " / " + std::to_string(totalTutorialPages));
		world.GetComponent<Renderable>(tutorialImg).texture = &m_Game->GetTextureManager().Load("Tutorial/" + std::to_string(pageNumber));
		world.GetComponent<Renderable>(tutorialText).text->setString(PrefabGen::TutorialText(pageNumber));
		} });


	Entity goBackButton = world.CreateEntity();
	world.AddComponentToEntity<Transform>(goBackButton, Transform{ {625, 680} });
	world.AddComponentToEntity<Renderable>(goBackButton, Renderable{ {30, 30}, &m_Game->GetTextureManager().Load("UI/Button_X"), nullptr, RenderLayer::UI });
	world.AddComponentToEntity<Button>(goBackButton, Button{ {30, 30}, [&, darkOverlay, tutorialImg, tutorialText, goBackButton, pageInd, prevPageButton, nextPageButton]() {
		m_Game->GetSoundManager().PlaySound("Button_Click");
		world.DestroyEntity(darkOverlay);
		world.DestroyEntity(tutorialImg);
		world.DestroyEntity(tutorialText);
		world.DestroyEntity(goBackButton);
		world.DestroyEntity(pageInd);
		world.DestroyEntity(prevPageButton);
		world.DestroyEntity(nextPageButton);
		EnableButtons(true);
		} });
}

void MainMenuState::EnableButtons(bool enable) {
	World& world = m_Game->GetWorld();
	auto buttonVec = world.GetAllComponentsOfType<Button>();
	for (auto& [ent, buttonComp] : buttonVec) {
		buttonComp->isEnabled = enable;
	}
}