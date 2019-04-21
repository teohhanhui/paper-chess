#include "dotonborderpredicate.h"
#include "dot.h"

DotOnBorderPredicate::DotOnBorderPredicate(int left, int right, int top, int bottom)
    : m_left(left)
    , m_right(right)
    , m_top(top)
    , m_bottom(bottom)
{
}

bool DotOnBorderPredicate::operator()(const Dot *dot) const
{
    return (dot->x() == m_left
            || dot->x() == m_right
            || dot->y() == m_top
            || dot->y() == m_bottom);
}
