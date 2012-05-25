#include "dot.h"

//creates a dot object
Dot::Dot(int player, int dot_x, int dot_y, bool status):player(player), dot_x(dot_x), dot_y(dot_y),status(1)
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

int Dot::getStatus(){
    return status;
}

void Dot::setStatus(int x, int y, int currStat){
    //TO-DO
    //set status of dot, target dot using coordinates
    if(status == 1){
        status = 0;
    }
}
