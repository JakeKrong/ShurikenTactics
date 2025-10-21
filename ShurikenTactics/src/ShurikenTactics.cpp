// ShurikenTactics.cpp : Defines the entry point for the application.
//

#include <print>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include "World.h"
#include "Components.h"
#include "RenderSystem.h"
#include "InputSystem.h"
#include "TextureManager.h"

#include "Game.h"

int main()
{
	//World world;
	//sf::Clock clock;
	//Entity Louis = world.CreateEntity();
	//Entity Louis2 = world.CreateEntity();
	//TextureManager textMgr;

	//world.RegisterComponent<Transform>();
	//world.RegisterComponent<Renderable>();
	//world.RegisterComponent<Player>();

	//auto renderSys = world.RegisterSystem<RenderSystem>();
	//auto inputSys = world.RegisterSystem<InputSystem>();

	//Signature renderSignature;
	//renderSignature.set(world.GetComponentID<Renderable>());
	//renderSignature.set(world.GetComponentID<Transform>());
	//Signature inputSignature;
	//inputSignature.set(world.GetComponentID<Player>());

	//world.SetSystemSignature<RenderSystem>(renderSignature);
	//world.SetSystemSignature<InputSystem>(inputSignature);

	//renderSys->SetWorld(&world);
	//inputSys->SetWorld(&world);

	//Transform trans{ {200,200} };
	//Player player;

	//world.AddComponentToEntity(Louis, trans);
	//world.AddComponentToEntity(Louis2, trans);
	//world.AddComponentToEntity(Louis, player);

	//Renderable renderable{ {100.0f, 100.0f}, true, &textMgr.Load("louisEX_sheet.jpg"), true, {10, 7}, 70, 0.2f};
	////Renderable renderable2{ {100.0f, 100.0f}, true, &textMgr.Load("louisEX_sheet.jpg"), true, {10, 7}, 70, 0.2f, true };

	//world.AddComponentToEntity(Louis, renderable);
	////world.AddComponentToEntity(Louis2, renderable2);

	//auto renderWindow = sf::RenderWindow(sf::VideoMode({ 800,600 }), "Test Window", sf::Style::Close);
	//renderWindow.setFramerateLimit(60);

	//sf::RectangleShape overlay(sf::Vector2f(400, 300));
	//overlay.setFillColor(sf::Color(128, 128, 128));
	//overlay.setPosition({ 10,10 });
	//while (renderWindow.isOpen()) {
	//	float deltaTime = clock.restart().asSeconds();
	//	
	//	while (auto optEvent = renderWindow.pollEvent()) {
	//		const sf::Event& event = *optEvent;

	//		if (event.is<sf::Event::Closed>()) {
	//			renderWindow.close();
	//		}
	//	}
	//	//renderWindow.clear();
	//	//renderWindow.draw(overlay);
	//	renderSys->Update(renderWindow, deltaTime);
	//	inputSys->Update();
	//	renderWindow.display();
	//}
	//

	//return 0;

	Game game;
	game.Run();

	return 0;
}
