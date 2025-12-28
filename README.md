# Shuriken Tactics #

![ShurikenTactics_v1 0_Preview_R](https://github.com/user-attachments/assets/e99c2251-da11-458f-af5a-5ac2d37c4696)


************
**Shuriken Tactics** is a concept / learning project built in **C++** using **SFML 3.0** and a custom **Entity-Component-System (ECS)** architecture.  
The game is inspired by aiming & trajectory-focused gameplay you play as a ninja who throws ricocheting shurikens to defeat enemies across turns.

---

## 📥 Download
**[Click here to download the latest version (v1.0.1)](https://github.com/JakeKrong/ShurikenTactics/releases/tag/v1.0.1)**

1. Download the `ShurikenTactics.zip` file.
2. Extract all files to a folder.
3. Run `ShurikenTactics.exe` in the "\ShurikenTactics" folder.

## Project Status
> This project has officially had it's v1.0 released, but will still be under active development as a **learning and portfolio** piece.
> There are a total of *4* different levels for players to play and challenge. (as of v1.0.1)
> Built from scratch without engine dependencies, purely to understand the technical foundations behind gameplay systems.

---

🥷 Gameplay Concept
Inspired by trajectory-based combat, you play as a ninja navigating intricate environments. The core loop revolves around:

- Tactical Positioning: Move, jump, and drop across platforms to find the optimal angle of attack.
- Ricochet Mechanics: Throw shurikens that bounce off walls and obstacles, utilizing geometry to hit enemies behind cover.
- Strategic Diversity: Stages in the game are designed with different strategic viewpoints, requiring adaptilbility and planning to overcome.
If an enemy damages the player, the game is over.

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
- **Sounds System** – - Manages concurrent sound effects and looping background tracks with global volume control.
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
