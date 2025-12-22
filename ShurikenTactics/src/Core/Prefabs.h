#pragma once
#include "Game.h"
#include "Components.h"
#include <cassert>

//Test
#include <print>

namespace PrefabGen {
	//Can consider using on World and TextureManager instead of Game later on
	static Game* game = nullptr;

	inline void Init(Game* g) {
		game = g;
	}

	// ********** Environment Prefabs ********** //

	//If certain prefab parameters increase by a lot, can use struct as params instead to prevent bloat
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
		collider.entityColliders.push_back({ rectCollider , {}});
		collider.type = ColliderType::ObstacleBox;
		world.AddComponentToEntity<Collider>(wall, collider);

		return wall;
	}

	inline Entity Floor(sf::Vector2f position, sf::Vector2f size) {
		assert(game != nullptr && "World pointer is nullptr in prefabs generator!");
		World& world = game->GetWorld();
		Entity floor = world.CreateEntity();

		Transform transform;
		transform.position = position;
		world.AddComponentToEntity<Transform>(floor, transform);

		Renderable renderable;
		renderable.size = size;
		renderable.layer = RenderLayer::GameObject2;
		renderable.texture = &game->GetTextureManager().Load("Wood_Floor");
		world.AddComponentToEntity<Renderable>(floor, renderable);

		Collider collider;
		sf::FloatRect rectCollider{ position, size };
		collider.entityColliders.push_back({ rectCollider , {}});
		collider.type = ColliderType::ObstacleBox;
		world.AddComponentToEntity<Collider>(floor, collider);

		return floor;
	}

	inline Entity Platform(sf::Vector2f position, sf::Vector2f size) {
		assert(game != nullptr && "World pointer is nullptr in prefabs generator!");
		World& world = game->GetWorld();
		Entity floor = world.CreateEntity();

		Transform transform;
		transform.position = position;
		world.AddComponentToEntity<Transform>(floor, transform);

		Renderable renderable;
		renderable.size = size;
		renderable.layer = RenderLayer::GameObject2;
		renderable.texture = &game->GetTextureManager().Load("Wooden_Platform");
		world.AddComponentToEntity<Renderable>(floor, renderable);

		Collider collider;
		sf::FloatRect rectCollider{ position, {size.x, size.y / 2} };
		collider.entityColliders.push_back({ rectCollider , {} });
		collider.type = ColliderType::PlatformBox;
		world.AddComponentToEntity<Collider>(floor, collider);

		return floor;
	}

	// ********** Player / Player-Related Prefabs ********** //

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
		collider.entityColliders.push_back({ playerHitbox , {}});
		collider.type = ColliderType::PlayerBox;
		world.AddComponentToEntity<Collider>(player, collider);

		Physics physics;
		physics.mass = 25.0f;
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
		animationData.timeSinceLastFrame =	3.f;
		world.AddComponentToEntity<AnimationData>(shuriken, animationData);

		Collider collider;
		sf::CircleShape shurikenHitbox{ 15.0f };
		collider.entityColliders.push_back({ shurikenHitbox , {}});
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

		game->GetSoundManager().PlaySound("Shuriken_Throw");
		game->GetSoundManager().PlaySound("Shuriken_Slash");

		return shuriken;
	}

	// ********** Enemy Prefabs ********** //

	inline Entity Samurai(sf::Vector2f position, bool defaultFaceRight){
		assert(game != nullptr && "World pointer is nullptr in prefabs generator!");
		World& world = game->GetWorld();

		Entity samurai = world.CreateEntity();

		Transform transform;
		transform.position = position;
		world.AddComponentToEntity<Transform>(samurai, transform);

		Renderable renderable;
		renderable.size = { 300, 130 };
		renderable.layer = RenderLayer::GameObject1;
		renderable.texture = &game->GetTextureManager().Load("Samurai/Idle");
		world.AddComponentToEntity<Renderable>(samurai, renderable);

		AnimationData animData;
		game->GetTextureManager().SetAnimationData("Samurai/Idle", animData);
		world.AddComponentToEntity<AnimationData>(samurai, animData);

		Physics enemyPhy;
		enemyPhy.affectedByGravity = true;
		enemyPhy.mass = 10;
		world.AddComponentToEntity<Physics>(samurai, enemyPhy);

		Collider collider;
		sf::FloatRect enemyHitbox;
		enemyHitbox.position = { 400.0f,200.0f };
		enemyHitbox.size = { 80, 130 };
		collider.entityColliders.push_back({ enemyHitbox , {120,0} });
		collider.type = ColliderType::EnemyBox;
		world.AddComponentToEntity<Collider>(samurai, collider);

		Enemy enemy;
		enemy.origin = position;
		enemy.defaultFacingRight = defaultFaceRight;
		enemy.isFacingRight = defaultFaceRight;
		world.AddComponentToEntity<Enemy>(samurai, enemy);

		return samurai;
	}

	inline Entity Archer(sf::Vector2f position) {
		assert(game != nullptr && "World pointer is nullptr in prefabs generator!");
		World& world = game->GetWorld();

		Entity archer = world.CreateEntity();

		Transform transform;
		transform.position = position;
		world.AddComponentToEntity<Transform>(archer, transform);

		Renderable renderable;
		renderable.size = { 300, 200 };
		renderable.layer = RenderLayer::GameObject1;
		renderable.texture = &game->GetTextureManager().Load("Archer/Idle");
		world.AddComponentToEntity<Renderable>(archer, renderable);

		AnimationData animData;
		game->GetTextureManager().SetAnimationData("Archer/Idle", animData);
		world.AddComponentToEntity<AnimationData>(archer, animData);

		Physics enemyPhy;
		enemyPhy.affectedByGravity = true;
		enemyPhy.mass = 10;
		world.AddComponentToEntity<Physics>(archer, enemyPhy);

		Collider collider;
		sf::FloatRect enemyHitbox;
		enemyHitbox.position = { 400.0f,200.0f };
		enemyHitbox.size = { 100, 130 };
		collider.entityColliders.push_back({ enemyHitbox , {100,70} });
		collider.type = ColliderType::EnemyBox;
		world.AddComponentToEntity<Collider>(archer, collider);

		Enemy enemy;
		enemy.type = EnemyType::Archer;
		enemy.fovAngle = 120;
		world.AddComponentToEntity<Enemy>(archer, enemy);

		return archer;
	}

	inline Entity Arrow() {
		assert(game != nullptr && "World pointer is nullptr in prefabs generator!");
		World& world = game->GetWorld();

		Entity arrow = world.CreateEntity();

		Transform transform;
		world.AddComponentToEntity<Transform>(arrow, transform);

		Renderable renderable;
		renderable.size = { 50, 5 };
		renderable.layer = RenderLayer::GameObject1;
		renderable.texture = &game->GetTextureManager().Load("Archer/Arrow");
		world.AddComponentToEntity<Renderable>(arrow, renderable);

		Collider collider;
		sf::FloatRect arrowHitbox{ { 50, 5 }, {} };
		collider.entityColliders.push_back({ arrowHitbox , {} });
		collider.type = ColliderType::ProjectileBox;
		world.AddComponentToEntity<Collider>(arrow, collider);

		Physics physics;
		world.AddComponentToEntity<Physics>(arrow, physics);

		//std::function<void(Entity ent)> onArrowDestroyed = [&](Entity arrow) {
		//	World& world = game->GetWorld();

		//	Entity destroyEffect = world.CreateEntity();
		//	world.AddComponentToEntity<Transform>(destroyEffect, world.GetComponent<Transform>(arrow));

		//	Renderable renderable;
		//	renderable.size = { 30.0f, 30.0f };
		//	renderable.layer = RenderLayer::GameObject1;
		//	renderable.texture = &game->GetTextureManager().Load("Shuriken_Break");
		//	world.AddComponentToEntity<Renderable>(destroyEffect, renderable);

		//	AnimationData animationData;
		//	animationData.spriteSheetDim = { 3,1 };
		//	animationData.totalFrames = 3;
		//	animationData.frameTime = 0.05f;
		//	world.AddComponentToEntity<AnimationData>(destroyEffect, animationData);

		//	Lifetime lifetime;
		//	lifetime.remainingTime = animationData.frameTime * animationData.totalFrames;
		//	world.AddComponentToEntity<Lifetime>(destroyEffect, lifetime);

		//	};

		Lifetime lifetime;
		lifetime.durability = 1;
		//lifetime.OnDestroyedFunction = onArrowDestroyed;
		world.AddComponentToEntity<Lifetime>(arrow, lifetime);

		return arrow;
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
		collider.AddRect({ targetHitbox , {} });
		collider.type = ColliderType::TargetBox;
		world.AddComponentToEntity<Collider>(target, collider);

		Physics physics;
		physics.affectedByGravity = true;
		physics.mass = 30;
		world.AddComponentToEntity<Physics>(target, physics);

		Lifetime lifetime;
		lifetime.durability = 1;
		world.AddComponentToEntity<Lifetime>(target, lifetime);

		return target;
	}
}