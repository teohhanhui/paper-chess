#include "gameengine.h"
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <algorithm>
#include <stack>
#include "dot.h"
#include "line.h"
#include "dotcoordinatespredicate.h"
#include "lineendpointspredicate.h"

GameEngine::GameEngine(QObject *parent)
    : QObject(parent)
    , m_rows(0)
    , m_columns(0)
    , m_turnLimit(0)
    , m_turn(0)
    , m_currentPlayer(0)
{
}

GameEngine::~GameEngine()
{
    for (std::vector<Dot *>::iterator it = m_dots.begin(); it != m_dots.end(); ++it) {
        delete *it;
    }
}

int GameEngine::rows() const
{
    return m_rows;
}

int GameEngine::columns() const
{
    return m_columns;
}

int GameEngine::turnLimit() const
{
    return m_turnLimit;
}

int GameEngine::turnsLeft() const
{
    return m_turnLimit - m_turn;
}

int GameEngine::currentPlayer() const
{
    return m_currentPlayer;
}

GameEngine::Stage GameEngine::stage() const
{
    return m_stage;
}

const std::vector<Dot *> &GameEngine::getDots() const
{
    return m_dots;
}

void GameEngine::newGame(int rows, int columns, int turnLimit)
{
    m_rows = rows;
    m_columns = columns;
    m_turnLimit = turnLimit;
    m_turn = 0;
    m_currentPlayer = 0;
    m_stage = PlaceDotStage;

    m_pointDisabled.assign((rows + 1) * (columns + 1), false);

    emit gameStarted();
    emit turnsLeftChanged();
    emit currentPlayerChanged();
    emit stageChanged();
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

    if (!isPointActive(x, y)) {
        return false;
    }

    dot = new Dot(m_currentPlayer, x, y, true);
    m_dots.push_back(dot);
    qDebug() << "Placed dot: " << dot->x() << "," << dot->y();
    m_stage = ConnectDotsStage;
    emit stageChanged();
    return true;
}

bool GameEngine::connectDots(int x1, int y1, int x2, int y2)
{
    if (m_stage != ConnectDotsStage) {
        return false;
    }

    Dot *dot1 = findDot(m_dots, x1, y1);
    Dot *dot2 = findDot(m_dots, x2, y2);

    if (dot1 == 0 || dot2 == 0 || dot1 == dot2) {
        return false;
    }

    bool connected = false;

    if (dot1->player() == m_currentPlayer && dot1->isActive()
            && dot2->player() == m_currentPlayer && dot2->isActive()) {
        // both dots belong to the current player and are active

        if (dot1->isNeighbor(dot2)
                && findLine(dot1, dot2) == 0
                && !inChain(dot1, dot2)) {
            // the dots are neighbours and are not yet connected

            if (dot1->x() == dot2->x() || dot1->y() == dot2->y()) {
                // horizontal or vertical line

                connected = true;
            }
            else {
                //diagonal line

                Dot *blockingDot1 = findDot(m_dots, dot1->x(), dot2->y());
                Dot *blockingDot2 = findDot(m_dots, dot2->x(), dot1->y());

                if (blockingDot1 != 0 && blockingDot2 != 0) {
                    Line *blockingLine = findLine(blockingDot1, blockingDot2);

                    if (blockingLine == 0) {
                        connected = true;
                    }
                }
            }
        }
    }

    if (connected) {
        std::deque<Dot *> chain = insertIntoChain(dot1, dot2);
        checkChain(chain);
    }

    return connected;
}

void GameEngine::endTurn()
{
    m_turn++;
    m_currentPlayer = m_currentPlayer == 0 ? 1 : 0;
    m_stage = PlaceDotStage;
    emit turnsLeftChanged();
    emit currentPlayerChanged();
    emit stageChanged();
}

bool GameEngine::isPointActive(int x, int y) const
{
    return (!m_pointDisabled[y * (m_columns + 1) + x]);
}

bool GameEngine::inChain(Dot *dot1, Dot *dot2) const
{
    std::vector<std::deque<Dot *> >::const_iterator it;
    std::vector<std::deque<Dot *> >::const_iterator end = m_chains.end();
    const std::deque<Dot *> *chain;

    for (it = m_chains.begin(); it != end; ++it) {
        chain = &(*it);
        if (find(chain->begin(), chain->end(), dot1) != chain->end()
                && find(chain->begin(), chain->end(), dot2) != chain->end()) {
            return true;
        }
    }

    return false;
}

std::deque<Dot *> &GameEngine::insertIntoChain(Dot *dot1, Dot *dot2)
{
    std::vector<std::deque<Dot *> >::iterator it;
    std::vector<std::deque<Dot *> >::iterator end = m_chains.end();
    bool inserted = false;

    for (it = m_chains.begin(); !inserted && it != end; ++it) {
        std::deque<Dot *> &chain = *it;

        if (chain.front() == dot1) {
            chain.push_front(dot2);
            inserted = true;
        }
        else if (chain.front() == dot2) {
            chain.push_front(dot1);
            inserted = true;
        }
        else if (chain.back() == dot1) {
            chain.push_back(dot2);
            inserted = true;
        }
        else if (chain.back() == dot2) {
            chain.push_back(dot1);
            inserted = true;
        }

        if (inserted) {
            return chain;
        }
    }

    std::deque<Dot *> newChain;
    newChain.push_back(dot1);
    newChain.push_back(dot2);

    m_chains.push_back(newChain);
    return m_chains.back();
}

void GameEngine::checkChain(std::deque<Dot *> &chain)
{
    Dot *start = chain.front();
    Dot *end = chain.back();
    bool completed = false;
    bool surrounded = false;
    std::deque<Dot *> surroundingDots(chain);

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

bool GameEngine::closeGap(std::deque<Dot *> chain) const
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
    std::stack<Dot *> unvisited;
    std::vector<Dot *> visited;
    std::vector<Dot *> resultPath;
    std::vector<Dot *> connectedDots;
    bool pathFound = false;
    std::vector<Line *>::size_type totalLines = m_lines.size();
    Dot *currentDot;
    Dot *nextDot;
    std::vector<Dot *>::const_iterator it;
    std::vector<Dot *>::const_iterator end;

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

    return (dot->x() == 0 || dot->x() == m_columns
            || dot->y() == 0 || dot->y() == m_rows);
}

void GameEngine::linkChain()
{
    std::vector<Dot *>::const_iterator it;
    std::vector<Dot *>::const_iterator end = m_chain.end();
    std::vector<Dot *>::const_iterator next;

    for (it = m_chain.begin(); it != end; ++it) {
        next = it + 1;
        if (next != end) {
            m_lines.push_back(new Line(*it, *next));
        }
    }

    emit chainCompleted();
}

void GameEngine::captureArea(const std::deque<Dot *> surroundingDots)
{
    int captured=0, mX=0, mY=0, smX=0, smY=0, currX=0, currY=0;
    int rowct=0,colct=0;
    int capturedPlayer = m_currentPlayer == 0 ? 1 : 0;

    //getting min max x & y
    smX = surroundingDots[0]->x();
    smY = surroundingDots[0]->y();
    mX = surroundingDots[0]->x();
    mY = surroundingDots[0]->y();

    std::deque<Dot *>::const_iterator it;
    std::deque<Dot *>::const_iterator end = surroundingDots.end();

    for (it = surroundingDots.begin(); it != end; ++it) {
        currX = (*it)->x();
        currY = (*it)->y();

        if (currX < smX) {
            smX = currX;
        }
        if (currY < smY) {
            smY = currY;
        }
    }

    for (it = surroundingDots.begin(); it != end; ++it) {
        currX = (*it)->x();
        currY = (*it)->y();

        if (currX > mX) {
            mX = currX;
        }
        if (currY > mY) {
            mY = currY;
        }
    }

    //calculate captured dots
    for (rowct = smY; rowct < mY; ++rowct) {
        for (colct = smX; colct < mX; ++colct) {
            if (findDot(surroundingDots, rowct, colct)->player() == capturedPlayer) {
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

template <typename Container>
Dot *GameEngine::findDot(const Container &dots, int x, int y) const
{
    DotCoordinatesPredicate pred(x, y);
    typename Container::const_iterator it = find_if(dots.begin(), dots.end(), pred);

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
    std::vector<Line *>::const_iterator it = find_if(m_lines.begin(), m_lines.end(), pred);

    if (it != m_lines.end()) {
        return *it;
    }
    else {
        return 0;
    }
}

std::vector<Line *> GameEngine::findLines(const Dot *endpoint) const
{
    std::vector<Line *> matches;
    std::vector<Line *>::const_iterator it = m_lines.begin();
    std::vector<Line *>::const_iterator end = m_lines.end();
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

std::vector<Dot *> GameEngine::findConnectedDots(const Dot *dot) const
{
    std::vector<Dot *> connectedDots;
    const std::vector<Line *> &lines = findLines(dot);
    std::vector<Line *>::const_iterator it;
    std::vector<Line *>::const_iterator end = lines.end();

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
