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
				Transform& transComp = m_World->GetComponent<Transform>(ent);
				Button& buttonComp = m_World->GetComponent<Button>(ent);
				sf::FloatRect buttonArea{ {transComp.position}, {buttonComp.size} };
				sf::IntRect clickArea{ (sf::Vector2i)mousePos, {1, 1}};
				if (buttonArea.findIntersection((sf::FloatRect)clickArea)) {
					buttonComp.onButtonClick();
					buttonComp.buttonHeld = true;
					break;
				}
			}
		}
	}
	else if (const auto eventDetail = event.getIf<sf::Event::MouseButtonReleased>()) {
		if (eventDetail->button == sf::Mouse::Button::Left) {
			for (auto& ent : m_Entities) {
				Button& buttonComp = m_World->GetComponent<Button>(ent);
				buttonComp.buttonHeld = false;
			}
		}
	}
}

void UISystem::Update(sf::Vector2f mousePos) {
	if (!m_World) return;
	for (auto& ent : m_Entities) {
		Button& buttonComp = m_World->GetComponent<Button>(ent);
		if (buttonComp.buttonHeld && buttonComp.onButtonHeld) {
			buttonComp.onButtonHeld(mousePos);
		}
	}
}