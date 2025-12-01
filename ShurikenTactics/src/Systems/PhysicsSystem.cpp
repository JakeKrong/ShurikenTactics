#include "PhysicsSystem.h"
#include "World.h"

#include "Physics.h"
#include "Transform.h"
#include "Collider.h"
#include <iostream>

#define FixedUpdateStep static_cast<float>(1.0f/TargetFixedUpdateFrequency)

void PhysicsSystem::Update(const float& deltaTime) {
	if (!m_World) return;

	timeSinceFixedUpdate += deltaTime;

	//Fixed Update
	if (timeSinceFixedUpdate >= FixedUpdateStep) {
		timeSinceFixedUpdate -= FixedUpdateStep;
		for (Entity ent : m_Entities) {
			Physics& physicsComp = m_World->GetComponent<Physics>(ent);
			Transform& transComp = m_World->GetComponent<Transform>(ent);


			if (physicsComp.affectedByGravity && !physicsComp.isGrounded) {
				physicsComp.acceleration.y += 980.f; //GRAVITY
			}

			// Apply acceleration to velocity
			physicsComp.velocity += physicsComp.acceleration * FixedUpdateStep;

			// Update postion based on velocity & gravity
			transComp.position += physicsComp.velocity * FixedUpdateStep;

			//Reset Accelration
			physicsComp.acceleration = { 0,0 };
			physicsComp.isGrounded = false;
		}
	}
}