#ifndef DOT_H
#define DOT_H

class Dot{
public:
    Dot(int player, int dot_x, int dot_y, int status);
    int getPlayer();
    int getStatus();
    int getX();
    int getY();
    void setStatus(int x, int y, int currStat);

private:
    int player;
    int dot_x;//coordinates of dot
    int dot_y;
    int status;

};

#endif // DOT_H
