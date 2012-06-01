#include "dot.h"
#include <cstdlib>

Dot::Dot()
    : m_player(0)
    , m_x(-1)
    , m_y(-1)
    , m_active(false)
{
}

Dot::Dot(int player, int x, int y, bool active)
    : m_player(player)
    , m_x(x)
    , m_y(y)
    , m_active(active)
{
}

int Dot::player() const
{
    return m_player;
}

int Dot::x() const
{
    return m_x;
}

int Dot::y() const
{
    return m_y;
}

bool Dot::isValid() const
{
    return (m_x >= 0 && m_y >= 0);
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

    return (std::abs(m_x - other->x()) < 2
            && std::abs(m_y - other->y()) < 2);
}
