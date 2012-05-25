#ifndef DOT_H
#define DOT_H

class Dot {
public:
    Dot(int player, int x, int y, bool active = true);
    int getPlayer() const;
    int getX() const;
    int getY() const;
    bool isActive() const;
    void deactivate();

private:
    int player;
    int m_x;
    int m_y;
    int active;
};

#endif // DOT_H
