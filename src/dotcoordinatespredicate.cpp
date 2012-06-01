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

    return (dot->x() == m_x && dot->y() == m_y);
}
