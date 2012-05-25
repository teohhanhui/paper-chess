#ifndef DOTCOORDINATESPREDICATE_H
#define DOTCOORDINATESPREDICATE_H

#include "dot.h"

class DotCoordinatesPredicate
{
public:
    DotCoordinatesPredicate(int x, int y);
    bool operator()(const Dot *dot);
private:
    int m_x;
    int m_y;
};

#endif // DOTCOORDINATESPREDICATE_H
