#pragma once
#include <SFML/Graphics.hpp>

constexpr float radius = 7.f;
constexpr float gravity = 257.f;
constexpr float floorTensionR = 20;
constexpr float floorTension = 0.9f;

constexpr std::uint32_t windowHeight = 800;
constexpr std::uint32_t windowWidth = 700;

constexpr float screenH = static_cast<float>(windowHeight) - 10;
constexpr float screenW = static_cast<float>(windowWidth);

class Ball {
protected:
	sf::Vector2f position;
	sf::CircleShape ball;
	float radius = 1.f;
	sf::Color color;
	bool died = false;
	sf::Vector2f velocity = { 0.f, gravity };
	sf::Vector2f ogVelocity;

public:

	Ball(const sf::Color& color, const sf::Vector2f& pos, const float radius)
		: color(color), position(pos), radius(radius)
	{
		ball.setFillColor(color);
		setSize(radius);
		ball.setPosition(pos);
		ball.setOrigin({ radius, radius });
	}
	bool getDied()
	{
		return died;
	}

	void update(const float dt)
	{
		velocity.y += gravity * dt;
		move({ velocity.x * dt, velocity.y * dt });

		if (position.x >= screenW - radius)
		{
			velocity.x = -velocity.x * floorTension;
			position.x = screenW - radius;
		}
		else if (position.x <= radius)
		{
			position.x = radius;
			velocity.x = -velocity.x * floorTension;
		}
		else if (position.y + radius >= screenH)
		{
			if (velocity.x > 0)
			{
				velocity.x -= floorTensionR * dt;
				if (velocity.x < 0) velocity.x = 0;
			}
			else if (velocity.x < 0)
			{
				velocity.x += floorTensionR * dt;
				if (velocity.x > 0) velocity.x = 0;
			}

			velocity.y = -velocity.y * floorTension;
			position.y = screenH - radius;
			if (std::abs(velocity.y) < gravity * dt * 2.f)
			{
				velocity.y = 0.f;
				//position.y = screenH - radius;
			}
		}
		else if (position.y <= 0)
		{
			position.y = radius;
			velocity.y = -velocity.y;
		}
		if (std::abs(velocity.x) < 0.5f && std::abs(velocity.y) < 0.5f && position.y >= screenH - radius)
		{
			died = true;
		}
		else
		{
			died = false;
		}
	}
	const sf::Vector2f& getVelocity() const
	{
		return velocity;
	}
	const float getVelocityX() const
	{
		return velocity.x;
	}
	const float getVelocityY() const
	{
		return velocity.y;
	}
	void setVelocityX(float vel)
	{
		velocity.x = vel;
	}
	void setVelocityY(float vel)
	{
		velocity.y = vel;
	}

	void setPos(const sf::Vector2f& pos)
	{
		ball.setPosition(pos);
		position = pos;
	}
	const sf::Vector2f& getPos() const
	{
		return position;
	}
	void move(const sf::Vector2f& move)
	{
		ball.move(move);
		position += move;
	}
	void setSize(const float r)
	{
		ball.setRadius(r);
		radius = r;
		ball.setOrigin({ r, r });
	}
	const sf::FloatRect getGlobalBounds() const
	{
		return ball.getGlobalBounds();
	}
	void draw(sf::RenderWindow& window) const
	{
		window.draw(ball);
	}

	const float getRadius()const {
		return radius;
	}

	void setVelocity(sf::Vector2f newV)
	{
		velocity = newV;
	}

};