#pragma once
#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Vec2.h"
#include "EntityManager.h"
#include <fstream>

struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game
{
	sf::RenderWindow      window;
	EntityManager         entities;
	sf::Font              font;
	sf::Text              text;
	PlayerConfig          playerConfig;
	EnemyConfig           enemyConfig;
	BulletConfig          bulletConfig;
	sf::Clock             deltaClock;
	int                   score = 0;
	int                   currentFrame = 0;
	int                   lastEnemySpawnTime = 0;
	bool                  paused = false;
	bool                  running = true;

	void init(const std::string& config);
	void setPaused(bool paused);

	void Movement();
	void UserInput();
	void LifespanCount();
	void Render();
	void GUI();
	void EnemySpawner();
	void CheckCollisions();

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2f& mousePos);
	void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

	std::shared_ptr<Entity> player();

public:
	Game(const std::string& config);

	void run();
};