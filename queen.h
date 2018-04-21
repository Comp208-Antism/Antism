#pragma once

#include "ant.h"



class Queen : public Ant
{
public:
	Queen() {
		setFillColor(sf::Color::Red);
		setSize(sf::Vector2f(3, 3));
	}
	Queen(sf::Vector2f position) : Ant(position) {
		setPosition(position);

		setFillColor(sf::Color::Red);
		setSize(sf::Vector2f(3, 3));
	}

	void update(int frame) override {
		if (m_cooldown >= m_spawn_rate) {
			m_cooldown = 0.f;
			m_status = true;
		}
				
		m_cooldown++;
	}

	void move(sf::Vector2f offset) override {
		sf::Vector2f cp = getPosition();
		sf::Vector2f np = cp + offset;

		sf::Vector2i limit = Grid::GetSize();

		if (np.x < 0)
			np.x = cp.x + 1;
		else if (np.x >= limit.x)
			np.x = cp.x - 1;

		if (np.y < 0)
			np.y = cp.y + 1;
		else if (np.y >= limit.y)
			np.y = cp.y - 1;

		Grid::Assign((int)cp.y, (int)cp.x, { -4, nullptr, nullptr });
		Grid::Assign((int)np.y, (int)np.x, { -4, this, nullptr });

		setPosition(np);
	}

	void setStatus(bool status) {
		m_status = status;
	}

	bool getStatus() {
		return m_status;
	}

private:
	float m_spawn_rate = 20.f;
	float m_cooldown = 0.f;
	bool m_status = false;
};

