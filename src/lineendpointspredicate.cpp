#include "lineendpointspredicate.h"

LineEndpointsPredicate::LineEndpointsPredicate(const Dot *endpoint1, const Dot *endpoint2)
    : m_endpoint1(endpoint1)
    , m_endpoint2(endpoint2)
{
}

bool LineEndpointsPredicate::operator()(const Line *line)
{
    if (line == 0) {
        return false;
    }

    if (m_endpoint2 == 0) {
        // only match a single endpoint
        return (line->getEndpoint1() == m_endpoint1 || line->getEndpoint2() == m_endpoint1);
    }

    // match both endpoints in any order
    return ((line->getEndpoint1() == m_endpoint1 && line->getEndpoint2() == m_endpoint2)
            || (line->getEndpoint1() == m_endpoint2 && line->getEndpoint2() == m_endpoint1));
}
