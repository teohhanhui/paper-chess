#ifndef LINEENDPOINTSPREDICATE_H
#define LINEENDPOINTSPREDICATE_H

class Dot;
class Line;

class LineEndpointsPredicate
{
public:
    LineEndpointsPredicate(
        const Dot *endpoint1,
        const Dot *endpoint2 = nullptr);
    bool operator()(const Line *line);

private:
    const Dot *m_endpoint1;
    const Dot *m_endpoint2;
};

#endif // LINEENDPOINTSPREDICATE_H
