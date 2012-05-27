#include "dotcoordinatespredicate.h"

DotCoordinatesPredicate::DotCoordinatesPredicate(int x, int y)
    : m_x(x)
    , m_y(y)
{
}

bool DotCoordinatesPredicate::operator()(const Dot *dot)
{
    if (dot == 0) {
        return false;
    }

    return (dot->getX() == m_x && dot->getY() == m_y);
}
