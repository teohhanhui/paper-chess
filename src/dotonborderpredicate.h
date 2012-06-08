#ifndef DOTONBORDERPREDICATE_H
#define DOTONBORDERPREDICATE_H

class Dot;

class DotOnBorderPredicate
{
public:
    DotOnBorderPredicate(int left, int right, int top, int bottom);
    bool operator()(const Dot *dot) const;

private:
    int m_left;
    int m_right;
    int m_top;
    int m_bottom;
};

#endif // DOTONBORDERPREDICATE_H
