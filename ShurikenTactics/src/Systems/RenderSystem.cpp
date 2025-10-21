#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "RenderSystem.h"
#include  "World.h"
#include "Transform.h"
//Test
#include <iostream>

void RenderSystem::Update(sf::RenderWindow& renderWindow, const float& deltaTime) {
	if (!m_World) return;
	renderWindow.clear();
	for (const Entity& systemEnt : m_Entities) {
		const Transform& transformComp = m_World->GetComponent<Transform>(systemEnt);
		Renderable& renderComp = m_World->GetComponent<Renderable>(systemEnt);
		if (!renderComp.visible) continue;

		sf::RectangleShape renderable{ renderComp.size };
		//renderable.setScale(renderComp.size);
		renderable.setPosition(transformComp.position);
		renderable.setRotation(sf::degrees(transformComp.rotation));
		renderable.setOrigin({ 0.0f,0.0f });

		//Check if it has a texture
		if (renderComp.texture != nullptr){
			renderable.setTexture(renderComp.texture);
			//Check if it's animated
			if (renderComp.animated) {
				SetAnimationFrame(renderComp, deltaTime);
				renderable.setTextureRect(renderComp.activeSprite);
			}
		}
		//Set default shape and colour if no texture is found
		else {
			renderable.setFillColor(sf::Color::White);
		}

		if (!renderComp.flipX) {
			renderable.setScale(transformComp.scale);
		}
		else {
			renderable.setScale({ -transformComp.scale.x, transformComp.scale.y });
			renderable.setOrigin({ renderable.getOrigin().x + renderComp.size.x, renderable.getOrigin().y });
		}

		//Can add addtional logic to check for renderable layers before calling .draw()
		renderWindow.draw(renderable);
	}
	renderWindow.display();
}

void RenderSystem::SetAnimationFrame(Renderable& renderComp, const float& deltaTime) {

	renderComp.timeSinceLastFrame += deltaTime;

	if (renderComp.timeSinceLastFrame >= renderComp.frameTime) {
		renderComp.currentFrame = (renderComp.currentFrame + 1) % renderComp.totalFrames;

		const sf::Vector2u& textureDim = renderComp.texture->getSize();
		sf::Vector2u frameSize = {
			textureDim.x / renderComp.spriteSheetDim.x,
			textureDim.y / renderComp.spriteSheetDim.y
		};

		int column = renderComp.currentFrame % renderComp.spriteSheetDim.x;
		int row = renderComp.currentFrame / renderComp.spriteSheetDim.x;

		renderComp.activeSprite = {
			{column * static_cast<int>(frameSize.x), row * static_cast<int>(frameSize.y)},
			{static_cast<int>(frameSize.x), static_cast<int>(frameSize.y)}
		};

		renderComp.timeSinceLastFrame -= renderComp.frameTime;
	}
}

void RenderSystem::SetWorld(World* world) {
	ISystem::SetWorld(world);
}
