#include "engine.h"
#include <algorithm>
#include "dotcoordinatespredicate.h"

using namespace std;

GameEngine::GameEngine(QObject *parent)
    : QObject(parent)
{
}

GameEngine::~GameEngine()
{
    for (vector<Dot *>::iterator it = m_dots.begin(); it != m_dots.end(); ++it) {
        delete *it;
    }
}

bool GameEngine::placeDot(int x, int y)
{
    if (m_stage != PlaceDotStage) {
        return false;
    }

    Dot *dot = new Dot(m_currentPlayer, x, y, true);
    m_dots.push_back(dot);

    m_stage = ConnectingStage;
    return true;
}

bool GameEngine::connectDots(int x1, int y1, int x2, int y2)
{
    if (m_stage != ConnectingStage) {
        return false;
    }

    Dot *start = searchDot(m_dots, x1, y1);
    Dot *end = searchDot(m_dots, x2, y2);

    if ((start->getPlayer() == m_currentPlayer) && (end->getPlayer() == m_currentPlayer)) {
        if (m_chain.empty()) {
            m_chain.push_back(start);
            m_chain.push_back(end);
            return true;
        }
        else {
            // the start dot must be the previous end dot

            vector<Dot *>::iterator ite = find(m_chain.begin(), m_chain.end(), start);

            if (*ite == start) {
                m_chain.push_back(start);
                m_chain.push_back(end);
                return true;
            }
        }
    }

    return false;
}

bool GameEngine::chainCheck()
{
    int counter, fX, fY, lX, lY;

    //get coordinates for first dot
    fX = m_chain[0]->getX();
    fY = m_chain[0]->getY();

    counter = 0;
    //iterate through surround vector, check dots is from the same player & status of dot == 1
    //*** move to connectDots
    for (vector<Dot *>::iterator it = m_chain.begin(); it != m_chain.end(); ++it) {
        if (((*it)->getPlayer() != m_currentPlayer) && (*it)->isActive()) {
            return false;
        }
        counter++;
    }
    //***

    //get the coordinates of the last dot
    lX = m_chain[counter]->getX();
    lY = m_chain[counter]->getY();

    //if last dot == first dot, chain is created
    if((fX == lX) && (fY == lY)) {
        return true;
    }
    else {
        return false;
    }
}

int GameEngine::captureDot(const vector<Dot *> surroundingDots, int capturedPlayer)
{
    int captured=0, mX=0, mY=0, smX=0, smY=0, currX=0, currY=0;
    int rowct=0,colct=0;

    //getting min max x & y
    smX = surroundingDots[0]->getX();
    smY = surroundingDots[0]->getY();
    mX = surroundingDots[0]->getX();
    mY = surroundingDots[0]->getY();

    for (vector<Dot *>::const_iterator it = surroundingDots.begin(); it != surroundingDots.end(); ++it) {
        currX = (*it)->getX();
        currY = (*it)->getY();

        if (currX < smX) {
            smX = currX;
        }
        if (currY < smY) {
            smY = currY;
        }
    }

    for (vector<Dot *>::const_iterator it = surroundingDots.begin(); it != surroundingDots.end(); ++it) {
        currX = (*it)->getX();
        currY = (*it)->getY();

        if (currX > mX) {
            mX = currX;
        }
        if (currY > mY) {
            mY = currY;
        }
    }

    //calculate captured dots
    for (rowct=smY;rowct<mY;rowct++) {
        for (colct=smX;colct<mX;colct++) {
            if (searchDot(surroundingDots, rowct, colct)->getPlayer() == capturedPlayer) {
                eatDot(rowct, colct);
                captured++;
            }
        }
    }

    m_chain.clear();
    return captured;
}

Dot *GameEngine::searchDot(const vector<Dot *> dots, int x, int y) const
{
    vector<Dot *>::const_iterator it = find_if(dots.begin(), dots.end(), DotCoordinatesPredicate(x, y));

    if (it != dots.end()) {
        return *it;
    }
    else {
        return 0;
    }
}

void GameEngine::eatDot(int x, int y)
{
    Dot *dot = searchDot(m_dots, x, y);

    if (dot != 0) {
        dot->deactivate();
    }
}
