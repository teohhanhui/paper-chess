#ifndef LINE_H
#define LINE_H

#include "dot.h"

class Line
{
public:
    Line(Dot *endpoint1, Dot *endpoint2);
    Dot *getEndpoint1() const;
    Dot *getEndpoint2() const;

private:
    Dot *m_endpoint1;
    Dot *m_endpoint2;
};

#endif // LINE_H
