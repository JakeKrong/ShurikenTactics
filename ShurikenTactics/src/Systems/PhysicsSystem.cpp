#include "PhysicsSystem.h"
#include "World.h"

#include "Physics.h"
#include "Transform.h"
#include "Collider.h"

void PhysicsSystem::Update(const float& deltaTime) {
	if (!m_World) return;

	for (Entity ent : m_Entities) {
		Physics& physicsComp = m_World->GetComponent<Physics>(ent);
		Transform& transComp = m_World->GetComponent<Transform>(ent);


		if (physicsComp.affectedByGravity && !physicsComp.isGrounded) {
			physicsComp.acceleration.y += 980.f; //GRAVITY
		}

		// Apply acceleration to velocity
		physicsComp.velocity += physicsComp.acceleration * deltaTime;

		// Update postion based on velocity & gravity
		transComp.position += physicsComp.velocity * deltaTime;

		//Reset Accelration
		physicsComp.acceleration = {0,0};
		physicsComp.isGrounded = false;
	}
}