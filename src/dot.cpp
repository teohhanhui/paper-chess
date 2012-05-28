#include "dot.h"
#include <cstdlib>

using namespace std;

//creates a dot object
Dot::Dot(int player, int x, int y, bool active)
    : m_player(player)
    , m_x(x)
    , m_y(y)
    , m_active(active)
{
}

int Dot::getPlayer() const
{
    return m_player;
}

int Dot::getX() const
{
    return m_x;
}

int Dot::getY() const
{
    return m_y;
}

bool Dot::isActive() const
{
    return m_active;
}

void Dot::deactivate()
{
    if (m_active) {
        m_active = false;
    }
}

bool Dot::isNeighbor(Dot *other) const
{
    if (other == 0) {
        return false;
    }

    return (abs(m_x - other->getX()) < 2
            && abs(m_y - other->getY()) < 2);
}
