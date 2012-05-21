#include "dot.h"

//creates a dot object
Dot::Dot(int player, int dot_x, int dot_y):player(player), dot_x(dot_x), dot_y(dot_y),status(1)
{

}

int Dot::getPlayer(){
    return player;
}

int Dot::getX(){
    return dot_x;
}

int Dot::getY(){
    return dot_y;
}

void setStatus(){
    //TO-DO
    //set status of dot
}
