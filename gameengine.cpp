#include "gameengine.h"
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

int GameEngine::rows() const
{
    return m_rows;
}

void GameEngine::setRows(int rows)
{
    if (rows != m_rows) {
        m_rows = rows;
        // reset game state?
    }
}

int GameEngine::columns() const
{
    return m_columns;
}

void GameEngine::setColumns(int columns)
{
    if (columns != m_columns) {
        m_columns = columns;
        // reset game state?
    }
}

int GameEngine::turnLimit() const
{
    return m_turnLimit;
}

void GameEngine::setTurnLimit(int turnLimit)
{
    if (turnLimit != m_turnLimit) {
        m_turnLimit = turnLimit;
        // reset game state?
    }
}

bool GameEngine::placeDot(int x, int y)
{
    if (m_stage != PlaceDotStage) {
        return false;
    }

    Dot *dot = findDot(m_dots, x, y);

    if (dot != 0) {
        return false;
    }

    dot = new Dot(m_currentPlayer, x, y, true);
    m_dots.push_back(dot);

    m_stage = ConnectingStage;
    return true;
}

bool GameEngine::connectDots(int x1, int y1, int x2, int y2)
{
    if (m_stage != ConnectingStage) {
        return false;
    }

    Dot *start = findDot(m_dots, x1, y1);
    Dot *end = findDot(m_dots, x2, y2);

    if (start == 0 || end == 0 || start == end) {
        return false;
    }

    bool connected = false;

    if (start->getPlayer() == m_currentPlayer && start->isActive()
            && end->getPlayer() == m_currentPlayer && end->isActive()) {
        if (m_chain.empty()) {
            m_chain.push_back(start);
            m_chain.push_back(end);
            connected = true;
        }
        else {
            // the start dot must be the previous end dot

            vector<Dot *>::iterator ite = find(m_chain.begin(), m_chain.end(), start);

            if (*ite == start) {
                m_chain.push_back(end);
                connected = true;
            }
        }
    }

    if (connected) {
        checkChain();
    }

    return connected;
}

void GameEngine::checkChain()
{
    int size = m_chain.size();
    Dot *start = m_chain[0];
    Dot *end = m_chain[size - 1];
    bool completed = false;
    bool surrounded = false;
    vector<Dot *> surroundingDots(m_chain);

    if (start == end) {
        completed = true;
        surrounded = true;
    }
    else {
        if (closeChain(surroundingDots)) {
            completed = true;
            surrounded = true;
        }
        else if (isOnEdge(start) && isOnEdge(end)) {
            completed = true;
        }
    }

    if (completed) {
        linkChain();
        if (surrounded) {
            captureArea(surroundingDots);
        }
        m_chain.clear();
    }
}

bool GameEngine::isOnEdge(const Dot *dot) const
{
    if (dot == 0) {
        return false;
    }

    return (dot->getX() == 0 || dot->getX() == m_columns
            || dot->getY() == 0 || dot->getY() == m_rows);
}

bool GameEngine::closeChain(vector<Dot *> chain) const
{
    vector<Line *> lines;

    return true;
}

void GameEngine::linkChain()
{
    vector<Dot *>::const_iterator it;
    vector<Dot *>::const_iterator end = m_chain.end();
    vector<Dot *>::const_iterator next;

    for (it = m_chain.begin(); it != end; ++it) {
        next = it + 1;
        if (next != end) {
            m_lines.push_back(new Line(**it, **next));
        }
    }

    emit chainCompleted();
}

void GameEngine::captureArea(const vector<Dot *> surroundingDots)
{
    int captured=0, mX=0, mY=0, smX=0, smY=0, currX=0, currY=0;
    int rowct=0,colct=0;
    int capturedPlayer = m_currentPlayer == 1 ? 2 : 1;

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
            if (findDot(surroundingDots, rowct, colct)->getPlayer() == capturedPlayer) {
                eatDot(rowct, colct);
                captured++;
            }
        }
    }

    m_chain.clear();
}

Dot *GameEngine::findDot(const vector<Dot *> dots, int x, int y) const
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
    Dot *dot = findDot(m_dots, x, y);

    if (dot != 0) {
        dot->deactivate();
    }
}
