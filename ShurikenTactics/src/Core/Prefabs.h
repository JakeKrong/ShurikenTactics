#pragma once
#include "Game.h"
#include "Components.h"
#include <cassert>

namespace PrefabGen {
	static Game* game = nullptr;

	inline void Init(Game* g) {
		game = g;
	}

	inline Entity Wall(sf::Vector2f position, sf::Vector2f size) {
		assert(game != nullptr && "World pointer is nullptr in prefabs generator!");
		World& world = game->GetWorld();
		Entity wall = world.CreateEntity();

		Transform transform; 
		transform.position = position;
		world.AddComponentToEntity<Transform>(wall, transform);

		Renderable renderable;
		renderable.size = size;
		renderable.layer = RenderLayer::GameObject2;
		renderable.texture = &game->GetTextureManager().Load("Wooden_Pillar");
		world.AddComponentToEntity<Renderable>(wall, renderable);

		Collider collider;
		sf::FloatRect rectCollider{ position, size };
		collider.entityColliders.push_back(rectCollider);
		collider.type = ColliderType::ObstacleBox;
		world.AddComponentToEntity<Collider>(wall, collider);

		return wall;
	}

	inline Entity Platform(sf::Vector2f position, sf::Vector2f size) {
		assert(game != nullptr && "World pointer is nullptr in prefabs generator!");
		World& world = game->GetWorld();
		Entity platform = world.CreateEntity();

		Transform transform;
		transform.position = position;
		world.AddComponentToEntity<Transform>(platform, transform);

		Renderable renderable;
		renderable.size = size;
		renderable.layer = RenderLayer::GameObject2;
		renderable.texture = &game->GetTextureManager().Load("Wood_Floor");
		world.AddComponentToEntity<Renderable>(platform, renderable);

		Collider collider;
		sf::FloatRect rectCollider{ position, size };
		collider.entityColliders.push_back(rectCollider);
		collider.type = ColliderType::ObstacleBox;
		world.AddComponentToEntity<Collider>(platform, collider);

		return platform;
	}

	inline Entity Player() {
		assert(game != nullptr && "World pointer is nullptr in prefabs generator!");
		World& world = game->GetWorld();

		Entity player = world.CreateEntity();

		Transform transform;
		transform.position = { 400.0f,500.0f };
		world.AddComponentToEntity<Transform>(player, transform);

		Renderable renderable;
		renderable.size = { 80, 100 };
		renderable.layer = RenderLayer::Player;
		renderable.texture = &game->GetTextureManager().Load("Player_Idle_Sprite");
		world.AddComponentToEntity<Renderable>(player, renderable);

		AnimationData animationData;
		animationData.spriteSheetDim = { 4,1 };
		animationData.totalFrames = 4;
		animationData.frameTime = 0.15f;
		world.AddComponentToEntity<AnimationData>(player, animationData);

		::Player playerComp;
		playerComp.health = 100;
		world.AddComponentToEntity<::Player>(player, playerComp);

		Collider collider;
		sf::FloatRect playerHitbox;
		playerHitbox.position = { 400.0f,100.0f };
		playerHitbox.size = { 80, 100 };
		collider.entityColliders.push_back(playerHitbox);
		collider.type = ColliderType::PlayerBox;
		world.AddComponentToEntity<Collider>(player, collider);

		Physics physics;
		physics.mass = 100.0f;
		physics.affectedByGravity = true;
		world.AddComponentToEntity<Physics>(player, physics);

		return player;
	}

	inline Entity Shuriken() {
		assert(game != nullptr && "World pointer is nullptr in prefabs generator!");
		World& world = game->GetWorld();

		Entity shuriken = world.CreateEntity();

		Transform transform;
		world.AddComponentToEntity<Transform>(shuriken, transform);

		Renderable renderable;
		renderable.size = { 30, 30 };
		renderable.layer = RenderLayer::GameObject1;
		renderable.texture = &game->GetTextureManager().Load("Shuriken");
		world.AddComponentToEntity<Renderable>(shuriken, renderable);

		AnimationData animationData;
		animationData.spriteSheetDim = { 3,1 };
		animationData.totalFrames = 3;
		animationData.frameTime = 0.03f;
		world.AddComponentToEntity<AnimationData>(shuriken, animationData);

		Collider collider;
		sf::CircleShape shurikenHitbox{ 15.0f };
		collider.entityColliders.push_back(shurikenHitbox);
		collider.type = ColliderType::ProjectileBox;
		world.AddComponentToEntity<Collider>(shuriken, collider);

		Physics physics;
		world.AddComponentToEntity<Physics>(shuriken, physics);

		std::function<void(Entity ent)> onShurikenDestroyed = [&](Entity shuriken) {
			World& world = game->GetWorld();

			Entity destroyEffect = world.CreateEntity();
			world.AddComponentToEntity<Transform>(destroyEffect, world.GetComponent<Transform>(shuriken));

			Renderable renderable;
			renderable.size = { 30.0f, 30.0f };
			renderable.layer = RenderLayer::GameObject1;
			renderable.texture = &game->GetTextureManager().Load("Shuriken_Break");
			world.AddComponentToEntity<Renderable>(destroyEffect, renderable);

			AnimationData animationData;
			animationData.spriteSheetDim = { 3,1 };
			animationData.totalFrames = 3;
			animationData.frameTime = 0.05f;
			world.AddComponentToEntity<AnimationData>(destroyEffect, animationData);

			Lifetime lifetime;
			lifetime.remainingTime = animationData.frameTime * animationData.totalFrames;
			world.AddComponentToEntity<Lifetime>(destroyEffect, lifetime);

			};

		Lifetime lifetime;
		lifetime.durability = 3;
		lifetime.OnDestroyedFunction = onShurikenDestroyed;
		world.AddComponentToEntity<Lifetime>(shuriken, lifetime);

		return shuriken;
	}

	inline Entity Target() {
		assert(game != nullptr && "World pointer is nullptr in prefabs generator!");
		World& world = game->GetWorld();

		Entity target = world.CreateEntity();

		Transform transform;
		world.AddComponentToEntity<Transform>(target, transform);

		Renderable renderable;
		renderable.size = { 60, 100 };
		renderable.layer = RenderLayer::GameObject1;
		renderable.texture = &game->GetTextureManager().Load("Strawman_Target");
		world.AddComponentToEntity<Renderable>(target, renderable);

		Collider collider;
		sf::FloatRect targetHitbox;
		targetHitbox.position = { 0,0 };
		targetHitbox.size = { 60,100 };
		collider.AddRect(targetHitbox);
		collider.type = ColliderType::TargetBox;
		world.AddComponentToEntity<Collider>(target, collider);

		Physics physics;
		physics.affectedByGravity = true;
		physics.mass = 100;
		world.AddComponentToEntity<Physics>(target, physics);

		Lifetime lifetime;
		lifetime.durability = 1;
		world.AddComponentToEntity<Lifetime>(target, lifetime);

		return target;
	}
}