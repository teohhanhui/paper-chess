#include "dot.h"
#include "line.h"
#include "engine.h"

using namespace std;
Engine::Engine(QObject *parent):QObject(parent){

}
//untested
int Engine::surroundCheck(vector<Dot> chain, int player){

    int counter, fX, fY, lX, lY;

    fX = chain[0].getX();
    fY = chain[0].getY();
    counter = 0;
    for(vector<Dot>::iterator ite = chain.begin(); ite != chain.end(); ++ite){
        if(((*ite).getPlayer() != player) && ((*ite).getStatus() != 1)){
            return 0;
        }
        counter++;
    }

    lX = chain[counter].getX();
    lY = chain[counter].getY();
    if((fX == lX) && (fY == lY)){
        return 1;
    }
    else{
        return 0;
    }
}

int Engine::captureDot(vector<Dot> surroundingDots, int capturedPlayer){
    int captured=0, mX=0, mY=0, smX=0, smY=0, currX=0, currY=0;
    int rowct=0,colct=0;
    //getting min max x & y
    smX = surroundingDots[0].getX();
    smY = surroundingDots[0].getY();
    mX = surroundingDots[0].getX();
    mY = surroundingDots[0].getY();
    for(vector<Dot>::iterator ite = surroundingDots.begin(); ite != surroundingDots.end(); ++ite){
        currX = (*ite).getX();
        currY = (*ite).getY();

        if(currX < smX){
            smX = currX;
        }
        if(currY < smY){
            smY = currY;
        }
    }
    for(vector<Dot>::iterator ite = surroundingDots.begin(); ite != surroundingDots.end(); ++ite){
        currX = (*ite).getX();
        currY = (*ite).getY();

        if(currX > mX){
            mX = currX;
        }
        if(currY > mY){
            mY = currY;
        }
    }
    //calculate captured dots
    for(rowct=smY;rowct<mY;rowct++){
        for(colct=smX;colct<mX;colct++){
            if(searchDot(surroundingDots, rowct, colct) == capturedPlayer){
                eatDot(surroundingDots, rowct, colct);
                captured++;
            }
        }
    }

    //return results
    return captured;
}

int Engine::searchDot(vector<Dot> surroundDots, int x, int y){
    int player=0;
    for(vector<Dot>::iterator ite = surroundDots.begin(); ite != surroundDots.end(); ++ite){
        if((*ite).getX() == x && (*ite).getY() == y){
            player = (*ite).getPlayer();
        }
    }
    return player;
}

void Engine::eatDot(vector<Dot> surroundDots, int x, int y){
    int status=0;
    for(vector<Dot>::iterator ite = surroundDots.begin(); ite != surroundDots.end(); ++ite){
        if((*ite).getX() == x && (*ite).getY() == y){
            status = (*ite).getStatus();
            if(status == 1){
                (*ite).setStatus(x, y, status);
            }
        }
    }

}
