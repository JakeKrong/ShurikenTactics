#pragma once
#include "Game.h"
#include "Components.h"
#include <cassert>

//Test
#include <print>
#include <iostream>

struct PrefabContext {
	World& world;
	TextureManager* textures;
	SoundManager* sound;
	FontManager* font;
};

namespace PrefabGen {

	// ********** Environment Prefabs ********** //

	//If certain prefab parameters increase by a lot, can use struct as params instead to prevent bloat

	inline Entity Wall(const PrefabContext& prefCtx, sf::Vector2f position, sf::Vector2f size) {
		World& world = prefCtx.world;
		Entity wall = world.CreateEntity();

		Transform transform;
		transform.position = position;
		world.AddComponentToEntity<Transform>(wall, transform);

		Renderable renderable;
		renderable.size = size;
		renderable.layer = RenderLayer::GameObject2;
		renderable.texture = &prefCtx.textures->Load("Wooden_Pillar");
		world.AddComponentToEntity<Renderable>(wall, std::move(renderable));

		Collider collider;
		sf::FloatRect rectCollider{ position, size };
		collider.entityColliders.push_back({ rectCollider , {} });
		collider.type = ColliderType::ObstacleBox;
		world.AddComponentToEntity<Collider>(wall, collider);

		return wall;
	}

	inline Entity Floor(const PrefabContext& prefCtx, sf::Vector2f position, sf::Vector2f size) {
		World& world = prefCtx.world;
		Entity floor = world.CreateEntity();

		Transform transform;
		transform.position = position;
		world.AddComponentToEntity<Transform>(floor, transform);

		Renderable renderable;
		renderable.size = size;
		renderable.layer = RenderLayer::GameObject2;
		renderable.texture = &prefCtx.textures->Load("Wood_Floor");
		world.AddComponentToEntity<Renderable>(floor, std::move(renderable));

		Collider collider;
		sf::FloatRect rectCollider{ position, size };
		collider.entityColliders.push_back({ rectCollider , {}});
		collider.type = ColliderType::ObstacleBox;
		world.AddComponentToEntity<Collider>(floor, collider);

		return floor;
	}

	inline Entity Platform(const PrefabContext& prefCtx, sf::Vector2f position, sf::Vector2f size) {
		World& world = prefCtx.world;
		Entity floor = world.CreateEntity();

		Transform transform;
		transform.position = position;
		world.AddComponentToEntity<Transform>(floor, transform);

		Renderable renderable;
		renderable.size = size;
		renderable.layer = RenderLayer::GameObject2;
		renderable.texture = &prefCtx.textures->Load("Wooden_Platform");
		world.AddComponentToEntity<Renderable>(floor, std::move(renderable));

		Collider collider;
		sf::FloatRect rectCollider{ position, {size.x, size.y / 2} };
		collider.entityColliders.push_back({ rectCollider , {} });
		collider.type = ColliderType::PlatformBox;
		world.AddComponentToEntity<Collider>(floor, collider);

		return floor;
	}

	inline Entity SidedPlatform(const PrefabContext& prefCtx, sf::Vector2f position, sf::Vector2f size, bool flip = false) {
		World& world = prefCtx.world;
		Entity floor = world.CreateEntity();

		Transform transform;
		transform.position = position;
		world.AddComponentToEntity<Transform>(floor, transform);

		Renderable renderable;
		renderable.size = size;
		renderable.layer = RenderLayer::GameObject2;
		renderable.texture = &prefCtx.textures->Load("Wooden_Platform_Sided");
		renderable.flipX = flip;
		world.AddComponentToEntity<Renderable>(floor, std::move(renderable));

		Collider collider;
		sf::FloatRect rectCollider{ position, {size.x, size.y / 2} };
		collider.entityColliders.push_back({ rectCollider , {} });
		collider.type = ColliderType::PlatformBox;
		world.AddComponentToEntity<Collider>(floor, collider);

		return floor;
	}

	inline Entity StaticDoor(const PrefabContext& prefCtx, sf::Vector2f position, bool flipRend) {
		World& world = prefCtx.world;
		Entity door = world.CreateEntity();

		Transform transform;
		transform.position = position;
		world.AddComponentToEntity<Transform>(door, transform);

		Renderable renderable;
		renderable.size = { 640, 720 };
		renderable.layer = RenderLayer::UI;
		renderable.texture = &prefCtx.textures->Load("Effects/Sliding_Door");
		renderable.flipX = flipRend;
		world.AddComponentToEntity<Renderable>(door, std::move(renderable));

		Lifetime lifetime;
		lifetime.remainingTime = 0.6f;
		world.AddComponentToEntity<Lifetime>(door, lifetime);

		return door;
	}

	inline Entity SlidingDoor(const PrefabContext& prefCtx, sf::Vector2f position, bool flipRend, bool velocity) {
		World& world = prefCtx.world;
		Entity door = world.CreateEntity();

		Transform transform;
		transform.position = position;
		world.AddComponentToEntity<Transform>(door, transform);

		Renderable renderable;
		renderable.size = {640, 720};
		renderable.layer = RenderLayer::UI;
		renderable.texture = &prefCtx.textures->Load("Effects/Sliding_Door");
		renderable.flipX = flipRend;
		world.AddComponentToEntity<Renderable>(door, std::move(renderable));

		Physics physics;
		velocity ? physics.velocity.x = 700 : physics.velocity.x = -700;
		world.AddComponentToEntity<Physics>(door, physics);

		Lifetime lifetime;
		lifetime.remainingTime = 0.93f;
		world.AddComponentToEntity<Lifetime>(door, lifetime);

		return door;
	}

	// ********** Player / Player-Related Prefabs ********** //

	inline Entity Player(const PrefabContext& prefCtx, sf::Vector2f position) {
		World& world = prefCtx.world;

		Entity player = world.CreateEntity();

		Transform transform;
		transform.position = position;
		world.AddComponentToEntity<Transform>(player, transform);

		Renderable renderable;
		renderable.size = { 80, 100 };
		renderable.layer = RenderLayer::Player;
		renderable.texture = &prefCtx.textures->Load("Player_Idle_Sprite");
		world.AddComponentToEntity<Renderable>(player, std::move(renderable));

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
		playerHitbox.position = position;
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

	inline Entity Shuriken(const PrefabContext& prefCtx) {
		World& world = prefCtx.world;

		Entity shuriken = world.CreateEntity();

		Transform transform;
		world.AddComponentToEntity<Transform>(shuriken, transform);

		Renderable renderable;
		renderable.size = { 30, 30 };
		renderable.layer = RenderLayer::GameObject1;
		renderable.texture = &prefCtx.textures->Load("Shuriken");
		world.AddComponentToEntity<Renderable>(shuriken, std::move(renderable));

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

		std::function<void(Entity ent)> onShurikenDestroyed = [prefCtx](Entity shuriken) {
			World& world = prefCtx.world;

			Entity destroyEffect = world.CreateEntity();
			world.AddComponentToEntity<Transform>(destroyEffect, world.GetComponent<Transform>(shuriken));

			Renderable renderable;
			renderable.size = { 30.0f, 30.0f };
			renderable.layer = RenderLayer::GameObject1;
			renderable.texture = &prefCtx.textures->Load("Shuriken_Break");
			world.AddComponentToEntity<Renderable>(destroyEffect, std::move(renderable));

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

		prefCtx.sound->PlaySound("Shuriken_Throw");
		prefCtx.sound->PlaySound("Shuriken_Slash");

		return shuriken;
	}

	// ********** Enemy Prefabs ********** //

	inline Entity Samurai(const PrefabContext& prefCtx, sf::Vector2f position, bool defaultFaceRight, float fovAngle = 0){
		World& world = prefCtx.world;

		Entity samurai = world.CreateEntity();

		Transform transform;
		transform.position = position;
		world.AddComponentToEntity<Transform>(samurai, transform);

		Renderable renderable;
		renderable.size = { 300, 130 };
		renderable.layer = RenderLayer::GameObject1;
		renderable.texture = &prefCtx.textures->Load("Samurai/Idle");
		world.AddComponentToEntity<Renderable>(samurai, std::move(renderable));

		AnimationData animData;
		prefCtx.textures->SetAnimationData("Samurai/Idle", animData);
		world.AddComponentToEntity<AnimationData>(samurai, animData);

		Physics enemyPhy;
		enemyPhy.affectedByGravity = true;
		enemyPhy.mass = 10;
		world.AddComponentToEntity<Physics>(samurai, enemyPhy);

		Collider collider;
		sf::FloatRect enemyHitbox;
		enemyHitbox.position = position;
		enemyHitbox.size = { 80, 130 };
		collider.entityColliders.push_back({ enemyHitbox , {120,0} });
		collider.type = ColliderType::EnemyBox;
		world.AddComponentToEntity<Collider>(samurai, collider);

		Enemy enemy;
		enemy.origin = position;
		enemy.defaultFacingRight = defaultFaceRight;
		enemy.isFacingRight = defaultFaceRight;
		if (fovAngle != 0) enemy.fovAngle = fovAngle; //FOV Angle will be set as default (140) unless is specified
		world.AddComponentToEntity<Enemy>(samurai, enemy);

		return samurai;
	}

	inline Entity Archer(const PrefabContext& prefCtx, sf::Vector2f position, bool defaultFaceRight, float fovAngle = 0) {
		World& world = prefCtx.world;

		Entity archer = world.CreateEntity();

		Transform transform;
		transform.position = position;
		world.AddComponentToEntity<Transform>(archer, transform);

		Renderable renderable;
		renderable.size = { 300, 200 };
		renderable.layer = RenderLayer::GameObject1;
		renderable.texture = &prefCtx.textures->Load("Archer/Idle");
		world.AddComponentToEntity<Renderable>(archer, std::move(renderable));

		AnimationData animData;
		prefCtx.textures->SetAnimationData("Archer/Idle", animData);
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
		enemy.defaultFacingRight = defaultFaceRight;
		enemy.isFacingRight = defaultFaceRight;
		if (fovAngle != 0) enemy.fovAngle = fovAngle; //FOV Angle will be set as default (140) unless is specified
		world.AddComponentToEntity<Enemy>(archer, enemy);

		return archer;
	}

	inline Entity Arrow(const PrefabContext& prefCtx) {
		World& world = prefCtx.world;

		Entity arrow = world.CreateEntity();

		Transform transform;
		world.AddComponentToEntity<Transform>(arrow, transform);

		Renderable renderable;
		renderable.size = { 50, 5 };
		renderable.layer = RenderLayer::GameObject1;
		renderable.texture = &prefCtx.textures->Load("Archer/Arrow");
		world.AddComponentToEntity<Renderable>(arrow, std::move(renderable));

		Collider collider;
		sf::FloatRect arrowHitbox{ { 50, 5 }, {} };
		collider.entityColliders.push_back({ arrowHitbox , {} });
		collider.type = ColliderType::ProjectileBox;
		world.AddComponentToEntity<Collider>(arrow, collider);

		Physics physics;
		world.AddComponentToEntity<Physics>(arrow, physics);

		Lifetime lifetime;
		lifetime.durability = 1;
		world.AddComponentToEntity<Lifetime>(arrow, lifetime);

		return arrow;
	}


	inline Entity Target(const PrefabContext& prefCtx) {
		World& world = prefCtx.world;

		Entity target = world.CreateEntity();

		Transform transform;
		world.AddComponentToEntity<Transform>(target, transform);

		Renderable renderable;
		renderable.size = { 60, 100 };
		renderable.layer = RenderLayer::GameObject1;
		renderable.texture = &prefCtx.textures->Load("Strawman_Target");
		world.AddComponentToEntity<Renderable>(target, std::move(renderable));

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

	// ********** UI ********** //
	inline Entity Key(const PrefabContext& prefCtx, sf::Vector2f position ,const std::string& keyName) {
		World& world = prefCtx.world;

		Entity key = world.CreateEntity();

		Transform transform;
		world.AddComponentToEntity<Transform>(key, { position });

		Renderable renderable;
		renderable.size = { 30, 30 };
		renderable.layer = RenderLayer::UI;
		renderable.texture = &prefCtx.textures->Load("Controls/" + keyName);
		world.AddComponentToEntity<Renderable>(key, std::move(renderable));

		return key;
	}

	inline Entity Text(const PrefabContext& prefCtx, sf::Vector2f position, const std::string& textContents, int fontSize, sf::Color colour) {
		World& world = prefCtx.world;

		Entity key = world.CreateEntity();

		Transform transform;
		world.AddComponentToEntity<Transform>(key, { position });

		const sf::Font& font(prefCtx.font->Load("CENTAUR"));	
		sf::Text text(font);
		text.setString(textContents);
		text.setCharacterSize(fontSize);
		text.setFillColor(colour);
		text.setStyle(sf::Text::Bold);

		Renderable renderable;
		renderable.layer = RenderLayer::UI;
		renderable.text = std::make_unique<sf::Text>(text);

		world.AddComponentToEntity<Renderable>(key, std::move(renderable));

		return key;
	}

	inline std::string TutorialText(int screen) {
		switch (screen) {
		case 1:
			return ("In Shuriken Tactics, your SHURIKEN is the main tool to dispose of your enemies.\n\n"
					"Navigate the levels, outplay enemies, and defeat all threats to progress the game.\n\n"
					"[The current game version has a total of 4 levels, more will be added in the future]");
		case 2:
			return ("HOLD [Left Click] to aim your shuriken. When the yellow spark appears, RELEASE to fire.\n\n"
					"Shurikens can bounce up to TWO times off walls/floors");
		case 3:
			return ("Not all floors behave the same. Thinner platform can be jumped onto or dropped through using W and S.\n\n"
					"Use vertical movement to reposition and survive enemy attacks.");
		case 4:
			return ("Samurai are ruthless opponents. When SPOTTED on the same elevation, they will CHASE down and STRIKE\nthe player.\n"
					"If spotted on a different platform or preparing to shoot, the samurai will GUARD and PARRY incoming \n"
					"shurikens from the front, reflecting them back. [Deflected Shurikens] are LETHAL to the player.\n\n"
					"Shurikens hitting the RED AREA of the samurai will be deflected, and the GREEN AREA are the vulnerable spots.");
		case 5:
			return ("Archers are deadly at range. When player is spotted, they will draw their bow and fire towards your position\n"
					"(or your last seen location if player is out-of-sight).\n"
					"Just before firing, the aiming indicator will briefly flash red, signaling an imminent shot.\n\n"
					"Archers do not move, but getting too close is dangerous.\n"
					"When entering their melee range, they can put away their bow to unleash a swift, lethal slashing attack.");
		default:
			return "Lorem ipsum";
		}
	}
}