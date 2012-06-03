#include "line.h"

Line::Line(Dot &endpoint1, Dot &endpoint2)
    : m_endpoint1(endpoint1)
    , m_endpoint2(endpoint2)
{
}

Dot &Line::endpoint1() const
{
    return m_endpoint1;
}

Dot &Line::endpoint2() const
{
    return m_endpoint2;
}
