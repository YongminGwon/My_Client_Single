#include "Game.h"
#include "Component.h"
#include <random>
#include <cstdint>

Game::Game(const std::string& config) : font(), text(font, "My_Client")
{
	init(config);
}

void Game::init(const std::string& path)
{
	text.setCharacterSize(24);
	text.setFillColor(sf::Color::White);
	text.setPosition(Vec2f(100.f, 100.f));

	std::ifstream fin("config.txt");

	if (!fin.is_open())
	{
		std::cerr << "Can't Open config.txt\n";
		return;
	}

	std::string type;
	while (fin >> type)
	{
		if (type == "Window")
		{
			int width, height, framelimit, framestyle;
			fin >> width >> height >> framelimit >> framestyle;

			window.create(sf::VideoMode(sf::Vector2u(static_cast<unsigned>(width), static_cast<unsigned>(height))), "My_Client");
			window.setFramerateLimit(framelimit);
		}
		else if (type == "Font")
		{
			std::string fontFile;
			int size, r, g, b;
			fin >> fontFile >> size >> r >> g >> b;

			if (!font.openFromFile(fontFile))
			{
				throw std::runtime_error("Font load failed");
			}
			text.setFont(font);
			text.setCharacterSize(size);
			text.setFillColor(sf::Color(r, g, b));
		}
		else if (type == "Player")
		{
			fin >> playerConfig.SR >> playerConfig.CR >> playerConfig.S
				>> playerConfig.FR >> playerConfig.FG >> playerConfig.FB
				>> playerConfig.OR >> playerConfig.OG >> playerConfig.OB
				>> playerConfig.OT >> playerConfig.V;
		}
		else if (type == "Enemy")
		{
			fin >> enemyConfig.SR >> enemyConfig.CR
				>> enemyConfig.SMIN >> enemyConfig.SMAX
				>> enemyConfig.OR >> enemyConfig.OG >> enemyConfig.OB
				>> enemyConfig.OT >> enemyConfig.VMIN >> enemyConfig.VMAX
				>> enemyConfig.L >> enemyConfig.SI;
		}
		else if (type == "Bullet")
		{
			fin >> bulletConfig.SR >> bulletConfig.CR >> bulletConfig.S
				>> bulletConfig.FR >> bulletConfig.FG >> bulletConfig.FB
				>> bulletConfig.OR >> bulletConfig.OG >> bulletConfig.OB
				>> bulletConfig.OT >> bulletConfig.V >> bulletConfig.L;
		}
	}

	ImGui::SFML::Init(window);

	ImGui::GetStyle().ScaleAllSizes(2.0f);
	ImGui::GetIO().FontGlobalScale = 2.0f;

	spawnPlayer();
}

std::shared_ptr<Entity> Game::player()
{
	auto& players = entities.getEntities("player");
	return players.front();
}

void Game::run()
{
	//TODO : add pause funcionality in here
	//some systems should funciton while paused(rendering)
	//some systems shouldn't (movement / input)
	while (running)
	{
		ImGui::SFML::Update(window, deltaClock.restart());
		UserInput();

		if (!paused)
		{
			entities.update();
			Movement();
			EnemySpawner();
			CheckCollisions();
			LifespanCount();
			currentFrame++;
		}
		
		GUI();
		Render();
	}
}

void Game::setPaused(bool p)
{
	paused = p;
}

void Game::spawnPlayer()
{
	auto player = entities.addEntity("player");
	
	Vec2f position(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
	Vec2f velocity(0.0f, 0.0f);
	player->add<Transform>(position, velocity, 0.0f);

	player->add<Shape>(static_cast<float>(playerConfig.SR), playerConfig.V, sf::Color(playerConfig.FR, playerConfig.FG, playerConfig.FB), sf::Color(playerConfig.OR, playerConfig.OG, playerConfig.OB), static_cast<float>(playerConfig.OT));
	player->add<Collision>(static_cast<float>(playerConfig.CR));
	player->add<Input>();
	player->add<Score>();
}

void Game::spawnEnemy()
{
	//TODO : make sure the enemy is spawned properly with the enemyConfig variables the enemy must be spawned completely within the bounds of the window
	// the enemy must be spawned completely within the bounds of the window

	std::random_device rd;
	std::mt19937 gen(rd());

	auto enemy = entities.addEntity("enemy");

	float radius = static_cast<float>(enemyConfig.SR);
	float maxPosX = window.getSize().x - radius;
	float maxPosY = window.getSize().y - radius;
	float minPosX = static_cast<float>(enemyConfig.SR);
	float minPosY = static_cast<float>(enemyConfig.SR);

	std::uniform_real_distribution<float> disX(minPosX, maxPosX);
	std::uniform_real_distribution<float> disY(minPosY, maxPosY);

	float ranX = disX(gen);
	float ranY = disY(gen);

	std::uniform_real_distribution<float> disSpeed(enemyConfig.SMIN, enemyConfig.SMAX);
	float ranSpeed = disSpeed(gen);
	
	std::uniform_real_distribution<float> disAngle(0.0f, 360.0f);
	float ranAngle = disAngle(gen);
	float radians = ranAngle * (3.14159f/180.0f);

	Vec2f vel(std::cos(radians) * ranSpeed, std::sin(radians) * ranSpeed);

	enemy->add<Transform>(Vec2f(ranX, ranY), vel, ranAngle);

	std::uniform_int_distribution<int> disVertices(enemyConfig.VMIN, enemyConfig.VMAX);
    int vertices = disVertices(gen);

	enemy->add<Shape>(static_cast<float>(enemyConfig.SR), static_cast<size_t>(vertices), sf::Color(255, 255, 255), sf::Color(enemyConfig.OR, enemyConfig.OG, enemyConfig.OB), static_cast<float>(enemyConfig.OT));

	enemy->add<Collision>(static_cast<float>(enemyConfig.CR));

	enemy->add<Score>(100);

	enemy->add<Lifespan>(enemyConfig.L);

	//record when the most recent enemy was spawned
	lastEnemySpawnTime = currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
	//TODO : spawn small enemies at the location of the input enemy e

	//when we create the smaller enemy, we have to read the values of the original enemy
	//- spawn a number of small enemies equal to the vertices of the original enemy
	//- set each small enemy to the same color as the original, half the size
	// - small enemies are worth double points of the original enemy
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2f& mousePos)
{
	auto bullet = entities.addEntity("bullet");
	Vec2f entityPos = entity->get<Transform>().pos;

	Vec2f direction = mousePos - entityPos;
	float vecSize = std::sqrt(direction.x * direction.x + direction.y * direction.y);
	if(vecSize != 0 )
	{
		direction.x/=vecSize;
		direction.y/=vecSize;
	}
	float angle = std::atan2(direction.y, direction.x);

	Vec2f bulletVelocity = direction*bulletConfig.S;

	bullet->add<Transform>(entityPos, bulletVelocity, angle);
    bullet->add<Shape>(static_cast<float>(bulletConfig.SR), bulletConfig.V, sf::Color(bulletConfig.FR, bulletConfig.FG, bulletConfig.FB), sf::Color(bulletConfig.OR, bulletConfig.OG, bulletConfig.OB), static_cast<float>(bulletConfig.OT));
    bullet->add<Collision>(static_cast<float>(bulletConfig.CR));
    bullet->add<Lifespan>(bulletConfig.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
}

void Game::Movement()
{
	//TODO : implement all entity movements in this function
	// you should read the player->Input component to determine if the player is moving

	//sample movement speed update
	auto p = player();
	if(p->isActive())
	{
		auto& input = p->get<Input>();
		auto& transform = p->get<Transform>();

		float playerSpeed = playerConfig.S;

		Vec2f dir(0.0f, 0.0f);
		if(input.up) dir.y -= 1.0f;
		if(input.down) dir.y += 1.0f;
		if(input.left) dir.x -= 1.0f;
		if(input.right) dir.x += 1.0f;

		if(dir.x != 0.0f && dir.y != 0.0f)
		{
			const float diagonalMove = 0.7071f;
			dir.x *= diagonalMove;
			dir.y *= diagonalMove;
		}

		transform.velocity = dir * playerSpeed;

		transform.pos += transform.velocity;

		float radius = p->get<Shape>().circle.getRadius();
		transform.pos.x = std::max(radius, std::min(transform.pos.x, window.getSize().x - radius));
		transform.pos.y = std::max(radius, std::min(transform.pos.y, window.getSize().y - radius));
	}

	for(auto& e: entities.getEntities("enemy"))
	{
	    if(e->isActive())
		{
		    auto& transform = e->get<Transform>();
			transform.pos += transform.velocity;
			float radius = e->get<Shape>().circle.getRadius();

			if(transform.pos.x - radius < 0)
			{
				transform.pos.x = radius;
				transform.velocity.x *= -1;
			}
			 else if (transform.pos.x + radius > window.getSize().x)
            {
                transform.pos.x = window.getSize().x - radius;
                transform.velocity.x *= -1;
            }
            
            if (transform.pos.y - radius < 0)
            {
                transform.pos.y = radius;
                transform.velocity.y *= -1;
            }
            else if (transform.pos.y + radius > window.getSize().y)
            {
                transform.pos.y = window.getSize().y - radius;
                transform.velocity.y *= -1;
            }
			transform.angle += 1.0f;
		}
	}

	for (auto& b : entities.getEntities("bullet"))
    {
        if (b->isActive())
        {
            auto& transform = b->get<Transform>();
            
            transform.pos += transform.velocity;
            
            float radius = b->get<Shape>().circle.getRadius();
            if (transform.pos.x + radius < 0 || 
                transform.pos.x - radius > window.getSize().x ||
                transform.pos.y + radius < 0 || 
                transform.pos.y - radius > window.getSize().y)
            {
                b->destroy();
            }
        }
    }
}

void Game::LifespanCount()
{
	//TODO : implement all lifespan functionality
	//for all entities
	//if entity has no lifespan component, skip it
	//if entity has >0 remaining lifespan, subtract 1
	//if it has lifespan and is alive
	// scale its alpha channel properly
	//if it has lifespan and its time is up destroy the entity
	for(auto& entity : entities.getEntities())
	{
		if(entity->isActive() && entity->has<Lifespan>())
		{
			auto& lifespan = entity->get<Lifespan>();
			lifespan.remaining--;

			if(lifespan.remaining <= 0)
			{
				entity->destroy();
			}
			else if(entity->has<Shape>())
			{
				auto& shape = entity->get<Shape>();
				sf::Color fillColor = shape.circle.getFillColor();
				sf::Color outlineColor = shape.circle.getOutlineColor();

				float alphaRatio = static_cast<float>(lifespan.remaining) / lifespan.lifespan;
				std::uint8_t newAlpha = static_cast<std::uint8_t>(alphaRatio * 255);

				fillColor.a = newAlpha;
				outlineColor.a = newAlpha;

				shape.circle.setFillColor(fillColor);
				shape.circle.setOutlineColor(outlineColor);
			}
		}
	}
}

void Game::CheckCollisions()
{
	// TODO: implement all proper collisions between entities
	// be sure to use the collision radius
	
	// enemy bullet collision
	for(auto& bullet : entities.getEntities("bullet"))
	{
		if(!bullet->isActive()) continue;
		for(auto& enemy:entities.getEntities("enemy"))
		{
			if(!enemy->isActive()) continue;
			
			auto& bulletTransform = bullet->get<Transform>();
			auto& enemyTransform = enemy->get<Transform>();
			float bulletRadius = bullet->get<Collision>().radius;
			float enemyRadius = enemy->get<Collision>().radius;

			Vec2f diff = bulletTransform.pos - enemyTransform.pos;
			float distSquare = diff.x * diff.x + diff.y * diff.y;

			float collisionRadiusSquare = (bulletRadius + enemyRadius) * (bulletRadius + enemyRadius);

			if(distSquare < collisionRadiusSquare)
			{
				bullet->destroy();
				enemy->destroy();

				if(player()->has<Score>())
				{
					player()->get<Score>().score += enemy->get<Score>().score;
				}
				spawnSmallEnemies(enemy);
			}
		}
	}

	//player enemy collision
	auto p = player();
	if(p->isActive())
	{
		auto& playerTransform = p->get<Transform>();
		float playerRadius = p->get<Collision>().radius;

		for(auto& enemy:entities.getEntities("enemy"))
		{
			if(!enemy->isActive()) continue;

			auto& enemyTransform = enemy->get<Transform>();
			float enemyRadius = enemy->get<Collision>().radius;

			Vec2f diff = playerTransform.pos - enemyTransform.pos;
			float distSquare = diff.x * diff.x + diff.y * diff.y;

			float collisionRadiusSquare = (playerRadius + enemyRadius) * (playerRadius + enemyRadius);

			if(distSquare < collisionRadiusSquare)
			{
				enemy->destroy();
				// p->destroy();
				// TODO : Game Over Scene
			}
		}
	}
}

void Game::EnemySpawner()
{
	//TODO : code which implements enemy spawning should go here
	if(currentFrame >= lastEnemySpawnTime + enemyConfig.SI)
    {
		spawnEnemy();
	}
}

void Game::GUI()
{
	ImGui::Begin("Game Info");

	if (player()->isActive() && player()->has<Score>())
	{
		ImGui::Text("Score: %d", player()->get<Score>().score);
	}
	else
	{
		ImGui::Text("Score: N/A");
	}

	ImGui::Text("Frame: %lu", currentFrame);

	ImGui::Text("Active Enemies: %zu", entities.getEntities("enemy").size());
	ImGui::Text("Active Bullets: %zu", entities.getEntities("bullet").size());

	ImGui::End();
}

void Game::Render()
{
	//TODO : change the code below to draw ALL of the entities
	//sample drawing of the player entity that we have created
	window.clear();
	for(auto& entity : entities.getEntities())
	{
		if(entity->isActive() && entity->has<Shape>())
		{
			auto& transform = entity->get<Transform>();
			auto& shape = entity->get<Shape>();
			shape.circle.setPosition(transform.pos);
			shape.circle.setRotation(sf::degrees(transform.angle));
			window.draw(shape.circle);
		}
	}

	//draw the ui last
	GUI();
	ImGui::SFML::Render(window);
	window.display();
}

void Game::UserInput()
{
	//TODO : handle user input here
	// note that you should only be setting the player's input component
	//variables here
	//you should not implement the player's movement logic here
	// the movement system will read the variables you set in this function
	while (const std::optional event = window.pollEvent())
	{
		ImGui::SFML::ProcessEvent(window, *event);
		if (event->is<sf::Event::Closed>())
		{
			running = false;
			window.close();
		}

		if (event->is<sf::Event::KeyPressed>())
		{
			auto key = event->getIf<sf::Event::KeyPressed>()->code;
			switch (key)
			{
			case sf::Keyboard::Key::W:
				//TODO: set player's input component "up" to true
				player()->get<Input>().up = true;
				break;
			case sf::Keyboard::Key::A:
				player()->get<Input>().left = true;
				break;
			case sf::Keyboard::Key::S:
				player()->get<Input>().down = true;
				break;
			case sf::Keyboard::Key::D:
                player()->get<Input>().right = true;
                break;
			case sf::Keyboard::Key::P:
                setPaused(!paused);
                break;
			default: break;
			}
		}

		//this event is triggered when a key is released
		if (event->is<sf::Event::KeyReleased>())
		{
			auto key = event->getIf<sf::Event::KeyReleased>()->code;
			switch (key)
			{
			case sf::Keyboard::Key::W:
                player()->get<Input>().up = false;
                break;
            case sf::Keyboard::Key::A:
                player()->get<Input>().left = false;
                break;
            case sf::Keyboard::Key::S:
                player()->get<Input>().down = false;
                break;
            case sf::Keyboard::Key::D:
                player()->get<Input>().right = false;
                break;
            default: break;
			}
		}

		if (event->is<sf::Event::MouseButtonPressed>())
		{
			//this line ignores mouse events if ImGui is the thing being clicked
			if (ImGui::GetIO().WantCaptureMouse) { continue; }

			auto mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
			if (mouseEvent)
			{
				if (mouseEvent->button == sf::Mouse::Button::Left)
				{
					Vec2f mousePos(mouseEvent->position.x, mouseEvent->position.y);
                    spawnBullet(player(), mousePos);
				}
				else if (mouseEvent->button == sf::Mouse::Button::Right)
				{
					spawnSpecialWeapon(player());
				}
			}
		}
	}
}

