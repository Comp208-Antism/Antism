#include "ant.h"
#include "food.h"
#include "obstacle.h"

Ant::Ant()
{
	m_vertices[0].color = sf::Color::Green;
	m_vertices[1].color = sf::Color::Green;
	m_vertices[2].color = sf::Color::Green;
	m_vertices[3].color = sf::Color::Green;

	m_healthmax = float(rand() % 2500 + 2500);
	m_health = m_healthmax;
}

Ant::Ant(sf::Vector2f position)
{
	setPosition(position);

	m_vertices[0].color = sf::Color::Green;
	m_vertices[1].color = sf::Color::Green;
	m_vertices[2].color = sf::Color::Green;
	m_vertices[3].color = sf::Color::Green;

	m_healthmax = float(rand() % 2500 + 2500);
	m_health = m_healthmax;
}

Ant::~Ant()
{
}

void Ant::setBlanc() {
	Grid::Assign((int)m_vertices[0].position.y, (int)m_vertices[0].position.x, { -4, nullptr, nullptr });

	m_vertices[0] = { sf::Vector2f(0, 0), sf::Color(0, 0, 0, 0)};
	m_vertices[1] = { sf::Vector2f(0, 0), sf::Color(0, 0, 0, 0)};
	m_vertices[2] = { sf::Vector2f(0, 0), sf::Color(0, 0, 0, 0)};
	m_vertices[3] = { sf::Vector2f(0, 0), sf::Color(0, 0, 0, 0)};
}

void Ant::update() {
	// std::cout << m_health << std::endl;

	if (alive) {
		if (m_health <= 0) {
			alive = false;
			setBlanc();
		}
		else {
			if (!hasFood) {
				getFood();
			}
			else {
				goHome();
			}
		}

		m_health--;
	}
}

void Ant::foundFood(int y, int x) {
	hasFood = true;
	goingHome = true;
	m_health += m_healthmax / 8;

	auto f = ((Food*)Grid::Get(y, x).food);
	if (f->getCapacity() > 1) {
		f->lowerCapacity();
	}
	else {
		// std::cout << "food source depleted" << std::endl;
		f->despawn();
	}
	// std::cout << "food found" << std::endl;
}

void Ant::getFood() {
	//isHome = false;
	sf::Vector2f pos = getPosition();
	sf::Vector2i limit = Grid::GetSize();
	float x = pos.x;
	float y = pos.y;
	float dx = 0;
	float dy = 0;
	float scan[3][3];
	float sum = 0;

	// Path Selection Formula
	for (int a = 0; a < sizeof(*scan) / sizeof(**scan); a++) {
		for (int b = 0; b < sizeof(scan) / sizeof(*scan); b++) {
			if (!((x + (a - 1)) < 0 || (x + (a - 1)) >= limit.x || (y + (b - 1)) < 0 || (y + (b - 1)) >= limit.y)) {
				if (!(a == 1 && b == 1)) {
					if ((Grid::Get((int)(y + (b - 1)), (int)(x + (a - 1))).obstacle) == nullptr) {
						float pstr = Grid::GetGrid()[(int)(y + (b - 1))][(int)(x + (a - 1))].attributes.second;
						float sstr = Grid::GetGrid()[(int)(y + (b - 1))][(int)(x + (a - 1))].attributes.first;

						// Forward Directional Bias
						if (sstr > 0 || pstr > 0) {
							scan[a][b] = 1;
						}
						else {
							if (!m_trailoff.empty()) {
								sf::Vector2f pos = m_trailoff.front();
								int ox = pos.x;
								int oy = pos.y;

								if ((b - 1) == -oy && (a - 1) == -ox) {
									scan[a][b] = 0.7;
								}
								else if ((b - 1) == oy || (a - 1) == ox) {
									scan[a][b] = 0.5;
								}
								else {
									scan[a][b] = 0.6;
								}
							}
						}
							
						// Check if there is adjacent food
						if (((Food*)Grid::Get((int)(y + (b - 1)), (int)(x + (a - 1))).food)) {
							foundFood((int)(y + (b - 1)), (int)(x + (a - 1)));
						}

						// Apply Path Selection Formula value to cell 
						if ((pow(pstr, alpha) * pow(sstr, beta)) != 0) { // If there is adjacent pheromone AND adjacent smell
							scan[a][b] *= (pow(pstr, alpha) * pow(sstr, beta)) / (pow(pstr, alpha) + pow(sstr, beta));
						}
						else if ((pow(pstr, alpha) + pow(sstr, beta) == 0)) { // If there is no adjacent pheromone or smell
							scan[a][b] *= 1;
						}
						else {
							scan[a][b] *= pow(pow(pstr, alpha) + pow(sstr, beta), 0.5); // If there is only adjacent pheromone OR adjacent smell
						}

						// Calculate Sum of scan[][]
						sum += scan[a][b];

					}
					else {
						scan[a][b] = -1;
					}
				}
				else {
					scan[a][b] = -1;
				}
			}
			else {
				scan[a][b] = -1;
			}
		}
	}

	// Directional test (north-east)
	// scan[2][0] = 5;

	// Fitness Proportionate Selection (roulette wheel selection)
	float random = ((rand() % 100 + 1));
	float lower = 0;
	float upper = 0;
	for (int a = 0; a < sizeof(*scan) / sizeof(**scan); a++) {
		for (int b = 0; b < sizeof(scan) / sizeof(*scan); b++) {
			if (scan[a][b] != -1) {
				// Normalisation of scan[][] to range 0...100
				scan[a][b] /= (sum * pow(100, -1));

				// Upper and Lower bounds
				upper = lower + scan[a][b];
				if (lower < random && random <= upper) {
					dx = (int)(a - 1);
					dy = (int)(b - 1);
				}
				lower += scan[a][b];
			}
		}
	}

	move(sf::Vector2f((float)dx, (float)dy));
}

void Ant::goHome() {
	float dx = 0;
	float dy = 0;

	if (!m_trailoff.empty()) {
		sf::Vector2f pos = m_trailoff.back();
		m_trailoff.pop_back();
		dx = -pos.x;
		dy = -pos.y;

		move(sf::Vector2f((float)dx, (float)dy));
	}
	else {
		// std::cout << "home now" << std::endl;
		if (hasFood) {
			isHome = true;
			hasFood = false;
		}
		goingHome = false;
		getFood();
	}
}

float Ant::getHealth() {
	return m_health;
}

void Ant::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.draw(m_vertices, 4, sf::Quads);
}

void Ant::move(sf::Vector2f offset) {
	sf::Vector2f cp = getPosition();
	sf::Vector2f np = cp + offset;
	sf::Vector2i limit = Grid::GetSize();

	setPosition(np);
/*
	if (cp.x < 1 || cp.x >= limit.x - 1 || np.y < 1 || cp.y >= limit.y - 1) {
		goingHome = true;
	}
*/
	m_trail.push_back(sf::Vector2i(cp));

	if (!hasFood && !goingHome) {
		m_trailoff.push_back(sf::Vector2f(offset));
	}
	else {
		Grid::Assign((int)cp.y, (int)cp.x, { -4, nullptr, nullptr,{ 0.f, m_pheromone } });
		Grid::Assign((int)np.y, (int)np.x, { -4, this, nullptr });
	}

	m_pvertices.insert(m_pvertices.begin(), { sf::Vector2f(cp.x, cp.y) + sf::Vector2f(0, 0), sf::Color::Cyan });
	m_pvertices.insert(m_pvertices.begin(), { sf::Vector2f(cp.x, cp.y) + sf::Vector2f(1, 0), sf::Color::Cyan });
	m_pvertices.insert(m_pvertices.begin(), { sf::Vector2f(cp.x, cp.y) + sf::Vector2f(1, 1), sf::Color::Cyan });
	m_pvertices.insert(m_pvertices.begin(), { sf::Vector2f(cp.x, cp.y) + sf::Vector2f(0, 1), sf::Color::Cyan });
}

void Ant::setPosition(sf::Vector2f position)
{
	m_vertices[0].position = position + sf::Vector2f(0, 0);
	m_vertices[1].position = position + sf::Vector2f(m_size, 0);
	m_vertices[2].position = position + sf::Vector2f(m_size, m_size);
	m_vertices[3].position = position + sf::Vector2f(0, m_size);
}
