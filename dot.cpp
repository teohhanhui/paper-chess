#include "dot.h"

//creates a dot object
Dot::Dot(int player, int dot_x, int dot_y, int status):player(player), dot_x(dot_x), dot_y(dot_y),status(1)
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

int setStatus(int currStatus){
    //TO-DO
    //set status of dot
    if(currStatus == 1){
        currStatus = 0;
    }
    else if(currStatus == 0){
        currStatus = 1;
    }
    return currStatus;
}
