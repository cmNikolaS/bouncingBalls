#include<iostream>
#include <SFML\Graphics.hpp>
#include <vector>
#include <cmath>
#include <string>
#include "ball.hpp"

constexpr int maxWheel = 4;

int ballsPerLevel(const int level)
{
	int balls = 0;
	if (level == 1) balls = 1;
	else if (level == 2) balls = 5;
	else if (level == 3) balls = 10;
	else balls = 30;
	return balls;
}

void drawBalls(sf::RenderWindow& window, const std::vector<Ball> &balls)
{
	for (const auto& ball : balls)
		ball.draw(window);
}

void updateBalls(std::vector<Ball>& balls, const float dt)
{
	for (auto& ball : balls)
	{
		ball.update(dt);
	}
}

void resolveCollision(Ball& a, Ball& b)
{
	sf::Vector2f delta = b.getPos() - a.getPos();
	float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);

	if (dist == 0.f) return;

	float overlap = (a.getRadius() + b.getRadius()) - dist;

	sf::Vector2f normal = delta / dist;

	a.setPos(a.getPos() - normal * (overlap / 2.f));
	b.setPos(b.getPos() + normal * (overlap / 2.f));

	sf::Vector2f relVel = b.getVelocity() - a.getVelocity();

	float velAlongNormal = relVel.x * normal.x + relVel.y * normal.y;

	if (velAlongNormal > 0) return;

	float restitution = 1;

	float impulse = -(1 + restitution) * velAlongNormal;
	impulse /= 2;

	sf::Vector2f impulseVec = impulse * normal;

	a.setVelocity(a.getVelocity() - impulseVec);
	b.setVelocity(b.getVelocity() + impulseVec);
}

void updateBallsCollision(std::vector<Ball> &balls, const float dt)
{
	for (int i =0; i< balls.size(); i++)
	{
		for (int j = i+1; j < balls.size(); j++)
		{
			if (balls[i].getGlobalBounds().findIntersection(balls[j].getGlobalBounds()))
			{
				resolveCollision(balls[i], balls[j]);
			}
		}
	}
}

void clearBalls(std::vector<Ball> &balls)
{
	balls.erase(std::remove_if(balls.begin(), balls.end(), [](Ball& ball) {return ball.getDied(); }), balls.end());
}

int main()
{
	sf::RenderWindow window(sf::VideoMode({ windowWidth, windowHeight }), "BALL PHYSICS");

	std::vector<Ball> balls;
	balls.reserve(10000);

	sf::Clock clock;

	window.setFramerateLimit(165);

	bool moving = false;
	sf::Vector2f mousePos = { 0,0 };
	sf::Color c;

	sf::Font font;
	font.openFromFile("cursyger.ttf");

	int wheelLevel = 1;

	while (window.isOpen())
	{
		float dt = clock.restart().asSeconds();
		window.clear();
		bool mousePressedLeft = false, mousePressedRight = false;
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
			if (window.hasFocus())
			{
				if (auto mouseMovement = event->getIf<sf::Event::MouseButtonPressed>())
				{
					if (mouseMovement->button == sf::Mouse::Button::Left)
					{
						mousePressedLeft = true;
						mousePos = { (float)mouseMovement->position.x, (float)mouseMovement->position.y };
					}
					if (mouseMovement->button == sf::Mouse::Button::Right)
					{
						mousePressedRight = true;
						mousePos = { (float)mouseMovement->position.x, (float)mouseMovement->position.y };
					}
				}
				if (auto mouseMovement = event->getIf<sf::Event::MouseWheelScrolled>())
				{
					if (mouseMovement->wheel == sf::Mouse::Wheel::Vertical)
					{
						if (mouseMovement->delta > 0)
						{
							wheelLevel++;
							if (wheelLevel > maxWheel) wheelLevel = maxWheel;
						}
						else if (mouseMovement->delta < 0)
						{
							wheelLevel--;
							if (wheelLevel < 1) wheelLevel = 1;
						}
					}
				}
			}
		}

		int numOfNewballs = ballsPerLevel(wheelLevel);

		if (mousePressedLeft)
		{
			auto spawnPos = mousePos;
			for (int i = 1; i <= numOfNewballs; i++)
			{
				float r = 1;
				switch ((int)(dt * 100000+i) % 4)
				{
				case 0: c = sf::Color::Green; r = 2; break;
				case 1: c = sf::Color::Blue; r = 1.3; break;
				case 2: c = sf::Color::Magenta; r = 0.3; break;
				case 3: c = sf::Color::Yellow; r = 0.7; break;
				default: c = sf::Color::Black; r = 1.7; break;
				}
				if (spawnPos.x > screenW / 2)
					spawnPos.x -= radius* i-i;
				else
					spawnPos.x += radius *i+i;
				balls.emplace_back(c, spawnPos, radius);
			}
		}
		else if (mousePressedRight)
		{
			balls.clear();
		}
		clearBalls(balls);
		updateBalls(balls, dt);
		updateBallsCollision(balls, dt);

		std::string textStr(
			"NUMBER OF BALLS: " + std::to_string(balls.size()) + "\n\n\n"
			"Press LEFT click to MAKE a ball\n"
			"Press RIGHT click to DELETE balls\n"
			"Use WHEEL to INCREASE/DECREASE \nnumber of balls made per click\n"
			"Number of Balls per click: " + std::to_string(numOfNewballs)
		);

		sf::Text text(font, textStr, 12);
		text.setPosition({ 30, 50 });
		
		drawBalls(window, balls);
		window.draw(text);

		window.display();
	}

	return 0;
}