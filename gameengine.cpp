#include "gameengine.h"
#include <algorithm>
#include <stack>
#include "dotcoordinatespredicate.h"
#include "lineendpointspredicate.h"

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
        if (closeGap(surroundingDots)) {
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

bool GameEngine::closeGap(vector<Dot *> chain) const
{
    if (chain.size() < 2) {
        return false;
    }

    Dot *startDot = chain[0];
    Dot *endDot = chain[chain.size() - 1];

    // check that the start and end dots are actually connected to something
    if (findLine(startDot) == 0 || findLine(endDot) == 0) {
        return false;
    }

    /*
     * BEGIN: find path using an iterative DFS algorithm
     */
    stack<Dot *> unvisited;
    vector<Dot *> visited;
    vector<Dot *> resultPath;
    vector<Dot *> connectedDots;
    bool pathFound = false;
    vector<Line *>::size_type totalLines = m_lines.size();
    Dot *currentDot;
    Dot *nextDot;
    vector<Dot *>::const_iterator it;
    vector<Dot *>::const_iterator end;

    // push the start dot onto unvisited stack
    unvisited.push(startDot);

    // mark the start dot as visited
    visited.push_back(startDot);

    // add the start dot to the path
    resultPath.push_back(startDot);

    while (!pathFound && visited.size() < totalLines && !unvisited.empty()) {
        currentDot = unvisited.top();

        // find all dots connected to the current dot
        connectedDots = findConnectedDots(currentDot);

        end = connectedDots.end();

        // check for the end dot
        if (find(connectedDots.begin(), connectedDots.end(), endDot) != end) {
            pathFound = true;
        }
        else {
            // find the next dot to visit
            for (it = connectedDots.begin(), nextDot = 0; nextDot == 0 && it != end; ++it) {
                // check for a dot that has not been visited
                if (find(visited.begin(), visited.end(), *it) == visited.end()) {
                    nextDot = *it;
                }
            }

            // check for dead end
            if (nextDot == 0) {
                // pop the current dot off the unvisited stack
                unvisited.pop();

                // remove the current dot from the path
                resultPath.pop_back();
            }
            else {
                // push the dot onto unvisited stack
                unvisited.push(nextDot);

                // mark the dot as visited
                visited.push_back(nextDot);

                // add the dot to the path
                resultPath.push_back(nextDot);
            }
        }
    }
    /*
     * END: find path using an iterative DFS algorithm
     */

    if (pathFound) {
        // add dots from path to close the chain
        chain.insert(chain.end(), resultPath.rbegin(), resultPath.rend());
    }

    return pathFound;
}

bool GameEngine::isOnEdge(const Dot *dot) const
{
    if (dot == 0) {
        return false;
    }

    return (dot->getX() == 0 || dot->getX() == m_columns
            || dot->getY() == 0 || dot->getY() == m_rows);
}

void GameEngine::linkChain()
{
    vector<Dot *>::const_iterator it;
    vector<Dot *>::const_iterator end = m_chain.end();
    vector<Dot *>::const_iterator next;

    for (it = m_chain.begin(); it != end; ++it) {
        next = it + 1;
        if (next != end) {
            m_lines.push_back(new Line(*it, *next));
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
                captureDot(rowct, colct);
                captured++;
            }
        }
    }

    m_chain.clear();
}

void GameEngine::captureDot(int x, int y)
{
    Dot *dot = findDot(m_dots, x, y);

    if (dot != 0) {
        dot->deactivate();
    }
}

Dot *GameEngine::findDot(const vector<Dot *> dots, int x, int y) const
{
    DotCoordinatesPredicate pred(x, y);
    vector<Dot *>::const_iterator it = find_if(dots.begin(), dots.end(), pred);

    if (it != dots.end()) {
        return *it;
    }
    else {
        return 0;
    }
}

Line *GameEngine::findLine(const Dot *endpoint1, const Dot *endpoint2) const
{
    LineEndpointsPredicate pred(endpoint1, endpoint2);
    vector<Line *>::const_iterator it = find_if(m_lines.begin(), m_lines.end(), pred);

    if (it != m_lines.end()) {
        return *it;
    }
    else {
        return 0;
    }
}

vector<Line *> GameEngine::findLines(const Dot *endpoint) const
{
    vector<Line *> matches;
    vector<Line *>::const_iterator it = m_lines.begin();
    vector<Line *>::const_iterator end = m_lines.end();
    LineEndpointsPredicate pred(endpoint);

    while (it != end) {
        it = find_if(it, end, pred);

        if (it != end) {
            matches.push_back(*it);
            it++; // start checking from next element
        }
    }

    return matches;
}

vector<Dot *> GameEngine::findConnectedDots(const Dot *dot) const
{
    vector<Dot *> connectedDots;
    const vector<Line *> &lines = findLines(dot);
    vector<Line *>::const_iterator it;
    vector<Line *>::const_iterator end = lines.end();

    for (it = lines.begin(); it != end; ++it) {
        if ((*it)->getEndpoint1() != dot) {
            connectedDots.push_back((*it)->getEndpoint1());
        }
        else {
            connectedDots.push_back((*it)->getEndpoint2());
        }
    }

    return connectedDots;
}
