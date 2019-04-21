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

bool operator==(const Dot &dot1, const Dot &dot2)
{
    return !operator!=(dot1, dot2);
}

bool operator!=(const Dot &dot1, const Dot &dot2)
{
    if (dot1.m_player != dot2.m_player) {
        return true;
    }

    if (dot1.m_x != dot2.m_x) {
        return true;
    }

    if (dot1.m_y != dot2.m_y) {
        return true;
    }

    if (dot1.m_active != dot2.m_active) {
        return true;
    }

    return false;
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

bool Dot::isNeighbor(const Dot &other) const
{
    return (std::abs(m_x - other.x()) < 2
            && std::abs(m_y - other.y()) < 2);
}
