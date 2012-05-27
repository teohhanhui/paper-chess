#include "line.h"

Line::Line(Dot *endpoint1, Dot *endpoint2)
    : m_endpoint1(endpoint1)
    , m_endpoint2(endpoint2)
{
}

Dot *Line::getEndpoint1() const
{
    return m_endpoint1;
}

Dot *Line::getEndpoint2() const
{
    return m_endpoint2;
}
