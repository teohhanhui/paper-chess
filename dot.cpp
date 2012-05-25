#include "dot.h"

//creates a dot object
Dot::Dot(int player, int x, int y, bool status)
    : player(player)
    , m_x(x)
    , m_y(y)
    , active(status)
{
}

int Dot::getPlayer() const
{
    return player;
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
    return active;
}

void Dot::deactivate()
{
    if (active) {
        active = false;
    }
}
