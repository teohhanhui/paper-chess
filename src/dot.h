#ifndef DOT_H
#define DOT_H

class Dot
{
public:
    Dot();
    Dot(int player, int x, int y, bool active = true);
    int player() const;
    int x() const;
    int y() const;
    bool isValid() const;
    bool isActive() const;
    void deactivate();
    bool isNeighbor(const Dot &other) const;

private:
    int m_player;
    int m_x;
    int m_y;
    int m_active;
};

#endif // DOT_H
