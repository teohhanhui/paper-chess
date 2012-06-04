#ifndef DOT_H
#define DOT_H

class Dot
{
public:
    Dot();
    Dot(int player, int x, int y, bool active = true);

    friend bool operator==(const Dot &dot1, const Dot &dot2);
    friend bool operator!=(const Dot &dot1, const Dot &dot2);

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

bool operator==(const Dot &dot1, const Dot &dot2);
bool operator!=(const Dot &dot1, const Dot &dot2);

#endif // DOT_H
