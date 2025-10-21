#include "UISystem.h"
#include "World.h"
#include <SFML/Graphics/Rect.hpp>
//Components
#include "Transform.h"
#include "Button.h"


void UISystem::HandleEvents(const sf::Event& event, const sf::Vector2f& mousePos) {
	if (!m_World) return;
	if (const auto eventDetail = event.getIf<sf::Event::MouseButtonPressed>()) {
		if (eventDetail->button == sf::Mouse::Button::Left) {
			for (auto& ent : m_Entities) {
				Transform transComp = m_World->GetComponent<Transform>(ent);
				Button buttonComp = m_World->GetComponent<Button>(ent);
				sf::FloatRect buttonArea{ {transComp.position}, {buttonComp.size} };
				sf::IntRect clickArea{ (sf::Vector2i)mousePos, {1, 1}};
				if (buttonArea.findIntersection((sf::FloatRect)clickArea)) {
					buttonComp.onButtonClick();
					break;
				}

			}
		}
	}
}