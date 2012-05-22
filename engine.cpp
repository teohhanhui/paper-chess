#include "dot.h"
#include "line.h"
#include "engine.h"

using namespace std;
Engine::Engine(QObject *parent):QObject(parent){

}

int surroundCheck(vector<Dot> chain, int player){

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

/*int captureDot(vector<Dot> surroundingDots){
    int captured, mX, mY, smX, smY, currX, currY;

    for(vector<Dot>::iterator ite = surroundingDots.begin(); ite != surroundingDots.end(); ++ite){

    }
}

Dot searchDot(vector<Dot> playerDots, Dot target){

}
*/
