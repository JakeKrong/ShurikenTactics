#include "RenderSystem.h"
#include  "World.h"
#include "Transform.h"

#define COLLIDER_RENDER_DEBUG_FLAG 0
#include "Collider.h"
#include <iostream>

void RenderSystem::Update(sf::RenderWindow& renderWindow, const float& deltaTime) {
	if (!m_World) return;
	renderWindow.clear();

	//Sort m_Entities sequence when change detected
	if (m_EntityVectorSize != m_Entities.size() || m_LastElement != m_Entities[m_Entities.size() - 1]) {
		m_EntityVectorSize = m_Entities.size();

		std::sort(m_Entities.begin(), m_Entities.end(),
			[&](const Entity& a, const Entity& b) {
				const Renderable& renderA = m_World->GetComponent<Renderable>(a);
				const Renderable& renderB = m_World->GetComponent<Renderable>(b);
				return renderA.layer < renderB.layer;
			});

			m_LastElement = m_Entities[m_EntityVectorSize - 1];
	}

	for (const Entity& systemEnt : m_Entities) {
		const Transform& transformComp = m_World->GetComponent<Transform>(systemEnt);
		Renderable& renderComp = m_World->GetComponent<Renderable>(systemEnt);
		if (!renderComp.visible) continue;

		sf::RectangleShape renderable{ renderComp.size };
		//renderable.setScale(renderComp.size);
		renderable.setPosition(transformComp.position);
		renderable.setRotation(sf::degrees(transformComp.rotation));
		renderable.setOrigin({ 0.0f,0.0f });
		renderable.setFillColor(renderComp.tint);

		//Check if it has a texture
		if (renderComp.texture != nullptr){
			renderable.setTexture(renderComp.texture);
			//Check if it's animated
			if (m_World->HasComponent<AnimationData>(systemEnt)) {
				AnimationData& animationComp = m_World->GetComponent<AnimationData>(systemEnt);
				if (animationComp.totalFrames > 1) {
					SetAnimationFrame(renderComp, animationComp, deltaTime);
					renderable.setTextureRect(animationComp.activeSprite);
				}
			}
		}

		if (!renderComp.flipX) {
			renderable.setScale(transformComp.scale);
		}
		else {
			renderable.setScale({ -transformComp.scale.x, transformComp.scale.y });
			renderable.setOrigin({ renderable.getOrigin().x + renderComp.size.x, renderable.getOrigin().y });
		}

		if (renderComp.transformFunction != nullptr) renderComp.transformFunction();

		renderWindow.draw(renderable);
	}

//Debug tool to outline collider shapes
#if COLLIDER_RENDER_DEBUG_FLAG
	//This function works for entities with Renderable component only, so can attach non-visible Renderable for testing
	for (const Entity& systemEnt : m_Entities) {
		if (m_World->HasComponent<Collider>(systemEnt)) {
			const Collider& entCollider = m_World->GetComponent<Collider>(systemEnt);
			for (auto& [collider, _] : entCollider.entityColliders) {
				std::visit([&](auto&& shape) {
					using T = std::decay_t<decltype(shape)>;

					if (std::is_same_v<T, sf::FloatRect>) {
						sf::FloatRect rectCol = std::get<sf::FloatRect>(collider);
						sf::RectangleShape renderable;
						renderable.setPosition(rectCol.position);
						renderable.setSize(rectCol.size);
						renderable.setFillColor(sf::Color::Transparent);
						renderable.setOutlineColor(sf::Color::Green);
						renderable.setOutlineThickness(1);
						renderWindow.draw(renderable);
					}
					else {
						sf::CircleShape circleCol = std::get<sf::CircleShape>(collider);
						circleCol.setFillColor(sf::Color::Transparent);
						circleCol.setOutlineColor(sf::Color::Red);
						circleCol.setOutlineThickness(1);
						renderWindow.draw(circleCol);
					}
					}, collider);
			}
		}
	}
#endif

	renderWindow.display();
}

void RenderSystem::SetAnimationFrame(const Renderable& renderableComp, AnimationData& animationComp, const float& deltaTime) {

	animationComp.timeSinceLastFrame += deltaTime;

	if (animationComp.timeSinceLastFrame >= animationComp.frameTime) {
		if (animationComp.currentFrame + 1 >= animationComp.totalFrames && !animationComp.loopAnim) {
			if (animationComp.currentFrame + 1 == animationComp.totalFrames) {
				animationComp.currentFrame += 1;
				if (animationComp.OnAnimationEnd != nullptr) animationComp.OnAnimationEnd();
			}
			return;
		}

		animationComp.currentFrame = (animationComp.currentFrame + 1) % animationComp.totalFrames;

		const sf::Vector2u& textureDim = renderableComp.texture->getSize();
		sf::Vector2u frameSize = {
			textureDim.x / animationComp.spriteSheetDim.x,
			textureDim.y / animationComp.spriteSheetDim.y
		};

		int column = animationComp.currentFrame % animationComp.spriteSheetDim.x;
		int row = animationComp.currentFrame / animationComp.spriteSheetDim.x;

		animationComp.activeSprite = {
			{column * static_cast<int>(frameSize.x), row * static_cast<int>(frameSize.y)},
			{static_cast<int>(frameSize.x), static_cast<int>(frameSize.y)}
		};

		//If have Animation Event on current frame, call function
		if (animationComp.animationEvents.find(animationComp.currentFrame + 1) != animationComp.animationEvents.end()) {
			animationComp.animationEvents.at(animationComp.currentFrame + 1)();
		}

		//animationComp.timeSinceLastFrame -= animationComp.frameTime;
		animationComp.timeSinceLastFrame = 0;
	}
}

void RenderSystem::SetWorld(World* world) {
	ISystem::SetWorld(world);
}