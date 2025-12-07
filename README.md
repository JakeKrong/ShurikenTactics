# Shuriken Tactics #

![Shuriken_Tactics_Preview_7-12-resized](https://github.com/user-attachments/assets/682f9e14-1c7d-4d44-a75e-92ab24ecd19a)



************
**Shuriken Tactics** is a concept / learning project built in **C++** using **SFML 3.0** and a custom **Entity-Component-System (ECS)** architecture.  
The game is inspired by aiming & trajectory-focused gameplay you play as a ninja who throws ricocheting shurikens to defeat enemies across turns.

---

## Project Status
> This project is currently in active development as a **learning and portfolio** piece.  
> Built from scratch without engine dependencies — purely to understand the technical foundations behind gameplay systems.

---

## Core Concept (To be implemented)
- Move, jump, or drop to traverse around the platforms to position strategically.
- Throw a number of **shurikens** that **ricochet** off walls and obstacles.
- Plan attacks strategically, as **enemies utilises their variety of movesets** to fight back the player.

If an enemy damages the player, the game is over.

The goal is to defeat the enemies by utilising the interplay between **trajectory**, **bouncing angles**, and **positioning**.

---

### Engine Architecture
- **Custom ECS framework** inspired by modern game engines.
  - `EntityManager` – handles creation and destruction of entities.
  - `ComponentManager` – manages data-driven components such as Transform, Physics, Renderable, Collider, etc.
  - `SystemManager` – runs logic for systems (Physics, Collision, Rendering, Input, etc.) based on entity signatures.
- Designed for scalability and reusability in future gameplay prototypes.

### Core Systems
- **Physics System** – integrates simple kinematics, gravity, and velocity updates.
- **Collision System** – AABB and circle collision detection, with flexible handler callbacks.
- **Render System** – Renders entities with SFML sprites, shapes, and text.
- **Input System** – Handles player input and interacts with entity components.
- **Lifetime System** – Manages entities lifetimes like projectiles, animations, etc...

---

## Learning Focus

This project serves as a personal deep dive into:
- Modern **C++ game architecture** (RAII, smart pointers, templates, ECS design).
- **SFML 3.0**
- **Gameplay logic and physics simulation**.
- Understanding **game systems modularity** for future reuse.

---

| Category        | Tools / Libraries |
|-----------------|-------------------|
| Language        | C++20             |
| Framework       | SFML 3.0          |
| Build System    | CMake             |
| IDE             | Visual Studio 2022|
| Version Control | Git / GitHub      |

---
