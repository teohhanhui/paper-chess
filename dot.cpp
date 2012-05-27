#include "dot.h"

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
