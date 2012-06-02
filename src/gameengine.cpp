#include "gameengine.h"
#include <QApplication>
#include <QStack>
#include <QDebug>
#include <algorithm>
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
    for (QList<Dot *>::iterator it = m_dots.begin(); it != m_dots.end(); ++it) {
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

const Dot *GameEngine::getDotAt(int x, int y) const
{
    return findDot(m_dots, x, y);
}

const QList<Dot *> &GameEngine::getDots() const
{
    return m_dots;
}

const QList<Line *> &GameEngine::getLines() const
{
    return m_lines;
}

bool GameEngine::canPlaceDot(int x, int y) const
{
    return (isPointActive(x, y) && findDot(m_dots, x, y) == 0);
}

bool GameEngine::canConnectDots(int x1, int y1, int x2, int y2) const
{
    Dot *dot1 = findDot(m_dots, x1, y1);
    Dot *dot2 = findDot(m_dots, x2, y2);

    // check if both dots exist and are not the same dot
    if (dot1 == 0 || dot2 == 0 || dot1 == dot2) {
        return false;
    }

    // check if both dots belong to the current player
    if (dot1->player() != m_currentPlayer || dot2->player() != m_currentPlayer) {
        return false;
    }

    // check if both dots are active
    if (!dot1->isActive() || !dot2->isActive()) {
        return false;
    }

    // check if the dots are neighbours and are not yet connected
    if (!dot1->isNeighbor(dot2)
            || findLine(dot1, dot2) != 0
            || connectedInChain(dot1, dot2)) {
        return false;
    }

    if (dot1->x() == dot2->x() || dot1->y() == dot2->y()) {
        // horizontal or vertical line

        return true;
    }
    else {
        //diagonal line

        Dot *blockingDot1 = findDot(m_dots, dot1->x(), dot2->y());
        Dot *blockingDot2 = findDot(m_dots, dot2->x(), dot1->y());

        if (blockingDot1 == 0 || blockingDot2 == 0) {
            return true;
        }

        Line *blockingLine = findLine(blockingDot1, blockingDot2);

        if (blockingLine == 0) {
            return true;
        }
    }

    return false;
}

void GameEngine::newGame(int rows, int columns, int turnLimit)
{
    m_rows = rows;
    m_columns = columns;
    m_turnLimit = turnLimit;
    m_turn = 0;
    m_currentPlayer = 0;
    m_stage = PlaceDotStage;

    m_pointDisabled = QBitArray((rows + 1) * (columns + 1), false);

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

    if (!canPlaceDot(x, y)) {
        return false;
    }

    Dot *dot = new Dot(m_currentPlayer, x, y, true);
    m_dots.push_back(dot);

    m_stage = ConnectDotsStage;
    emit stageChanged();
    return true;
}

bool GameEngine::connectDots(int x1, int y1, int x2, int y2)
{
    if (m_stage != ConnectDotsStage) {
        return false;
    }

    if (!canConnectDots(x1, y1, x2, y2)) {
        return false;
    }

    Dot *dot1 = findDot(m_dots, x1, y1);
    Dot *dot2 = findDot(m_dots, x2, y2);

    QList<Dot *> chain = insertIntoChain(dot1, dot2);
    checkChain(chain);

    return true;
}

void GameEngine::endTurn()
{
    m_turn++;
    m_currentPlayer = m_currentPlayer == 0 ? 1 : 0;
    m_stage = PlaceDotStage;
    emit turnsLeftChanged();
    emit currentPlayerChanged();
    emit stageChanged();

    m_chains.clear();
}

bool GameEngine::isPointActive(int x, int y) const
{
    return (!m_pointDisabled[y * (m_columns + 1) + x]);
}

bool GameEngine::connectedInChain(Dot *dot1, Dot *dot2) const
{
    const QList<QList<Dot *> > &chains = m_chains;
    QList<QList<Dot *> >::const_iterator chains_it;
    QList<QList<Dot *> >::const_iterator chains_end = chains.end();

    for (chains_it = chains.begin(); chains_it != chains_end; ++chains_it) {
        const QList<Dot *> &chain = *chains_it;
        QList<Dot *>::const_iterator it;
        QList<Dot *>::const_iterator end = chain.end();
        int indices[2];
        int i = 0;

        for (it = chain.begin(); it != end; ++it) {
            if (*it == dot1) {
                indices[0] = i;
            }
            else if (*it == dot2) {
                indices[1] = i;
            }
            i++;
        }

        if (std::abs(indices[0] - indices[1]) == 1) {
            return true;
        }
    }

    return false;
}

QList<Dot *> &GameEngine::insertIntoChain(Dot *dot1, Dot *dot2)
{
    QList<QList<Dot *> >::iterator it;
    QList<QList<Dot *> >::iterator end = m_chains.end();
    bool inserted = false;

    for (it = m_chains.begin(); !inserted && it != end; ++it) {
        QList<Dot *> &chain = *it;

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

    QList<Dot *> newChain;
    newChain.push_back(dot1);
    newChain.push_back(dot2);

    m_chains.push_back(newChain);
    return m_chains.back();
}

void GameEngine::checkChain(QList<Dot *> &chain)
{
    Dot *start = chain.front();
    Dot *end = chain.back();
    bool completed = false;
    bool surrounded = false;
    QList<Dot *> surroundingDots(chain);

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
        linkChain(chain);
        if (surrounded) {
            captureArea(surroundingDots);
        }
        m_chains.removeOne(chain);
    }
}

bool GameEngine::closeGap(QList<Dot *> chain) const
{
    if (chain.size() < 2) {
        return false;
    }

    Dot *startDot = chain.front();
    Dot *endDot = chain.back();

    // check that the start and end dots are actually connected to something
    if (findLine(startDot) == 0 || findLine(endDot) == 0) {
        return false;
    }

    /*
     * BEGIN: find path using an iterative DFS algorithm
     */
    QStack<Dot *> unvisited;
    QList<Dot *> visited;
    QList<Dot *> resultPath;
    bool pathFound = false;
    int totalLines = m_lines.size();
    Dot *currentDot;
    Dot *nextDot;

    // push the start dot onto unvisited stack
    unvisited.push(startDot);

    // mark the start dot as visited
    visited.push_back(startDot);

    // add the start dot to the path
    resultPath.push_front(startDot);

    while (!pathFound && visited.size() < totalLines && !unvisited.empty()) {
        currentDot = unvisited.top();

        // find all dots connected to the current dot
        const QList<Dot *> &connectedDots = findConnectedDots(currentDot);
        QList<Dot *>::const_iterator it;
        QList<Dot *>::const_iterator end = connectedDots.end();

        // check for the end dot
        if (std::find(connectedDots.begin(), connectedDots.end(), endDot) != end) {
            pathFound = true;
        }
        else {
            // find the next dot to visit
            for (it = connectedDots.begin(), nextDot = 0; nextDot == 0 && it != end; ++it) {
                // check for a dot that has not been visited
                if (std::find(visited.begin(), visited.end(), *it) == visited.end()) {
                    nextDot = *it;
                }
            }

            // check for dead end
            if (nextDot == 0) {
                // pop the current dot off the unvisited stack
                unvisited.pop();

                // remove the current dot from the path
                resultPath.pop_front();
            }
            else {
                // push the dot onto unvisited stack
                unvisited.push(nextDot);

                // mark the dot as visited
                visited.push_back(nextDot);

                // add the dot to the path
                resultPath.push_front(nextDot);
            }
        }
    }
    /*
     * END: find path using an iterative DFS algorithm
     */

    if (pathFound) {
        // add dots from path to close the chain
        chain += resultPath;
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

void GameEngine::linkChain(const QList<Dot *> &chain)
{
    QList<Dot *>::const_iterator it;
    QList<Dot *>::const_iterator end = chain.end();
    QList<Dot *>::const_iterator next;

    for (it = chain.begin(); it != end; ++it) {
        next = it + 1;
        if (next != end) {
            m_lines.push_back(new Line(*it, *next));
        }
    }

    emit chainCompleted();
}

void GameEngine::captureArea(const QList<Dot *> surroundingDots)
{/*
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
*/}

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
    typename Container::const_iterator it = std::find_if(dots.begin(), dots.end(), pred);

    if (it != dots.end()) {
        return *it;
    }
    else {
        return 0;
    }
}

Line *GameEngine::findLine(const Dot *endpoint1, const Dot *endpoint2) const
{
    const QList<Line *> &lines = m_lines;
    LineEndpointsPredicate pred(endpoint1, endpoint2);
    QList<Line *>::const_iterator it = std::find_if(lines.begin(), lines.end(), pred);

    if (it != lines.end()) {
        return *it;
    }
    else {
        return 0;
    }
}

QList<Line *> GameEngine::findLines(const Dot *endpoint) const
{
    QList<Line *> matches;
    const QList<Line *> &lines = m_lines;
    QList<Line *>::const_iterator it = lines.begin();
    QList<Line *>::const_iterator end = lines.end();
    LineEndpointsPredicate pred(endpoint);

    while (it != end) {
        it = std::find_if(it, end, pred);

        if (it != end) {
            matches.push_back(*it);
            it++; // start checking from next element
        }
    }

    return matches;
}

QList<Dot *> GameEngine::findConnectedDots(const Dot *dot) const
{
    QList<Dot *> connectedDots;
    const QList<Line *> &lines = findLines(dot);
    QList<Line *>::const_iterator it;
    QList<Line *>::const_iterator end = lines.end();

    for (it = lines.begin(); it != end; ++it) {
        const Line &line = **it;

        if (line.endpoint1() != dot) {
            connectedDots.push_back(line.endpoint1());
        }
        else {
            connectedDots.push_back(line.endpoint2());
        }
    }

    return connectedDots;
}
