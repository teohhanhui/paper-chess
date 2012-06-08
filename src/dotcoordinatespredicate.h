#ifndef DOTCOORDINATESPREDICATE_H
#define DOTCOORDINATESPREDICATE_H

class Dot;

class DotCoordinatesPredicate
{
public:
    DotCoordinatesPredicate(int x, int y);
    bool operator()(const Dot *dot) const;
private:
    int m_x;
    int m_y;
};

#endif // DOTCOORDINATESPREDICATE_H
