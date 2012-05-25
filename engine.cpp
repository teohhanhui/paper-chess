#include "dot.h"
#include "line.h"
#include "engine.h"

using namespace std;
Engine::Engine(QObject *parent):QObject(parent){

}

//placing dot
bool Engine::placeDot(int x, int y) {
    if (m_stage != PlaceDotStage){
        return false;
    }
    Dot currentDot(m_currentPlayer, x, y, true);
    m_dots.push_back(currentDot);

    m_stage = ConnectingStage;
    return true;
}

bool Engine::connectDots(int x1, int y1, int x2, int y2){
    Dot target_1;
    Dot target_2;
    if(m_stage != ConnectingStage){
        //not in connecting stage
        return false;
    }
    target_1 = searchDot(m_dots, x1, y1);
    target_2 = searchDot(m_dots, x2, y2);

    if((target_1.getPlayer() == m_currentPlayer) && (target_2.getPlayer() == m_currentPlayer))
    {
        if(m_chain.empty()){
            m_chain.push_back(target_1);
            m_chain.push_back(target_2);
            return true;
        }
        else{
            vector<Dot>::iterator ite;
            //can only draw 1 direction
            if(*find(m_chain.begin(),m_chain.end(),target_1) == target_1){
                m_chain.push_back(target_1);
                m_chain.push_back(target_2);
                return true;
            }
        }
    }
    else{
        //wrong dots
        return false;
    }
}



bool Engine::chainCheck(){

    int counter, fX, fY, lX, lY;

    //get coordinates for first dot
    fX = m_chain[0].getX();
    fY = m_chain[0].getY();
    counter = 0;
    //iterate through surround vector, check dots is from the same player & status of dot == 1
    //*** move to connectDots
    for(vector<Dot>::iterator ite = chain.begin(); ite != chain.end(); ++ite){
        if(((*ite).getPlayer() != m_currPlayer) && ((*ite).getStatus() != 1)){
            return false;
        }
        counter++;
    }
    //***

    //get the coordinates of the last dot
    lX = m_chain[counter].getX();
    lY = m_chain[counter].getY();

    //if last dot == first dot, chain is created
    if((fX == lX) && (fY == lY)){
        return true;
    }
    else{
        return false;
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
    //flush chain vector
    m_chain.clear();

    //return results
    return captured;
}

Dot Engine::searchDot(vector<Dot> dots, int x, int y){
    Dot target;
    int player=0;
    for(vector<Dot>::iterator ite = dots.begin(); ite != dots.end(); ++ite){
        if((*ite).getX() == x && (*ite).getY() == y){
            player = (*ite).getPlayer();
        }
    }
    if(player == m_currentPlayer){
        return target;
    }
}

void Engine::eatDot(vector<Dot> playerDot, int x, int y){
    int status=0;
    for(vector<Dot>::iterator ite = playerDot.begin(); ite != playerDot.end(); ++ite){
        if((*ite).getX() == x && (*ite).getY() == y){
            status = (*ite).getStatus();
            if(status == 1){
                (*ite).setStatus(x, y, status);
            }
        }
    }
}
