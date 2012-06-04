#include "gameengine.h"
#include <QApplication>
#include <QDebug>
#include <algorithm>
#include <stack>
#include <set>
#include <limits>
#include "dot.h"
#include "line.h"
#include "dotcoordinatespredicate.h"
#include "lineendpointspredicate.h"

GameEngine::GameEngine(QObject *parent)
    : QObject(parent)
    , m_numPlayers(DEFAULT_NUM_PLAYERS)
    , m_rows(0)
    , m_columns(0)
    , m_turnLimit(0)
    , m_turnsLeft(0)
    , m_currentPlayer(0)
{
    m_playerNames.resize(m_numPlayers);

    for (int i = 0; i < m_numPlayers; ++i) {
        m_playerNames[i] = "Player " + QString::number(i + 1);
    }

    m_playerScores.resize(m_numPlayers);

    for (int i = 0; i < m_numPlayers; ++i) {
        m_playerScores[i] = 0;
    }
}

GameEngine::~GameEngine()
{
    clearTurnData();
    clearGameData();
}

int GameEngine::numPlayers() const
{
    return m_numPlayers;
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
    return m_turnsLeft;
}

int GameEngine::currentPlayer() const
{
    return m_currentPlayer;
}

GameEngine::Stage GameEngine::stage() const
{
    return m_stage;
}

QVariantList GameEngine::playerNames() const
{
    QVariantList list;

    for (int i = 0; i < m_numPlayers; ++i) {
        list.append(m_playerNames[i]);
    }

    return list;
}

void GameEngine::setPlayerNames(QVariantList &list)
{
    if (list.size() == m_numPlayers) {
        m_playerNames.resize(m_numPlayers);

        for (int i = 0; i < m_numPlayers; ++i) {
            m_playerNames[i] = list.at(i).toString();
        }

        emit playerNamesChanged();
    }
}

QVariantList GameEngine::playerScores() const
{
    QVariantList list;

    for (int i = 0; i < m_numPlayers; ++i) {
        list.append(m_playerScores[i]);
    }

    return list;
}

const Dot *GameEngine::getDotAt(int x, int y) const
{
    return findDot(m_dots, x, y);
}

const std::deque<Dot *> &GameEngine::getDots() const
{
    return m_dots;
}

const std::vector<const Line *> GameEngine::getLines(int player) const
{
    std::vector<const Line *> outLines;
    const std::deque<Line *> &lines = m_lines;
    std::deque<Line *>::const_iterator it;
    std::deque<Line *>::const_iterator end = lines.end();
    Line *line;

    for (it = lines.begin(); it != end; ++it) {
        line = *it;

        if (line->endpoint1().player() == player) {
            outLines.push_back(line);
        }
    }

    return outLines;
}

const std::vector<const std::deque<Dot *>*> GameEngine::getChains() const
{
    std::vector<const std::deque<Dot *>*> outChains;
    const std::list<std::deque<Dot *>*> &chains = m_chains;
    std::list<std::deque<Dot *>*>::const_iterator it;
    std::list<std::deque<Dot *>*>::const_iterator end = chains.end();

    for (it = chains.begin(); it != end; ++it) {
        outChains.push_back(*it);
    }

    return outChains;
}

bool GameEngine::canPlaceDot(int x, int y) const
{
    return (isPointActive(x, y) && findDot(m_dots, x, y) == 0);
}

bool GameEngine::canConnectDots(int x1, int y1, int x2, int y2) const
{
    const Dot *dotPtr1 = findDot(m_dots, x1, y1);
    const Dot *dotPtr2 = findDot(m_dots, x2, y2);

    // check if both dots exist and are not the same dot
    if (dotPtr1 == 0 || dotPtr2 == 0 || dotPtr1 == dotPtr2) {
        return false;
    }

    const Dot &dot1 = *dotPtr1;
    const Dot &dot2 = *dotPtr2;

    // check if both dots belong to the current player
    if (dot1.player() != m_currentPlayer || dot2.player() != m_currentPlayer) {
        return false;
    }

    // check if both dots are active
    if (!dot1.isActive() || !dot2.isActive()) {
        return false;
    }

    // check if the dots are neighbours and are not yet connected
    if (!dot1.isNeighbor(dot2)
            || findLine(&dot1, &dot2) != 0
            || connectedInChain(dot1, dot2)) {
        return false;
    }

    if (dot1.x() == dot2.x() || dot1.y() == dot2.y()) {
        // horizontal or vertical line

        return true;
    }
    else {
        //diagonal line

        const Dot *blockingDot1 = findDot(m_dots, dot1.x(), dot2.y());
        const Dot *blockingDot2 = findDot(m_dots, dot2.x(), dot1.y());

        if (blockingDot1 == 0 || blockingDot2 == 0) {
            return true;
        }

        const Line *blockingLine = findLine(blockingDot1, blockingDot2);

        if (blockingLine == 0) {
            return true;
        }
    }

    return false;
}

void GameEngine::newGame(int rows, int columns, int turnLimit)
{
    clearGameData();
    clearTurnData();
    m_pointDisabled = QBitArray((rows + 1) * (columns + 1), false);

    m_rows = rows;
    m_columns = columns;
    emit gameStarted();

    m_turnLimit = turnLimit;
    m_turnsLeft = turnLimit;
    emit turnsLeftChanged();

    m_currentPlayer = 0;
    emit currentPlayerChanged();

    m_stage = PlaceDotStage;
    emit stageChanged();

    for (int i = 0; i < m_numPlayers; ++i) {
        m_playerScores[i] = 0;
    }
    emit playerScoresChanged();
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

    Dot &dot1 = *findDot(m_dots, x1, y1);
    Dot &dot2 = *findDot(m_dots, x2, y2);

    std::deque<Dot *> chain = addToChains(dot1, dot2);
    completeChain(chain);

    emit chainsChanged();

    return true;
}

void GameEngine::endTurn()
{
    if (m_turnsLeft <= 0) {
        return;
    }

    clearTurnData();
    emit turnEnded();

    if (m_currentPlayer == m_numPlayers - 1) {
        --m_turnsLeft;
        emit turnsLeftChanged();
    }

    if (m_turnsLeft > 0) {
        m_currentPlayer = ++m_currentPlayer == m_numPlayers ? 0 : m_currentPlayer;
        emit currentPlayerChanged();

        m_stage = PlaceDotStage;
        emit stageChanged();
    }
    else {
        m_stage = EndStage;
        emit stageChanged();
    }
}

bool GameEngine::isPointActive(int x, int y) const
{
    return (!m_pointDisabled[y * (m_columns + 1) + x]);
}

bool GameEngine::connectedInChain(const Dot &dot1, const Dot &dot2) const
{
    return (findChain(dot1, dot2) != 0);
}

std::deque<Dot *> &GameEngine::addToChains(Dot &dot1, Dot &dot2)
{
    std::list<std::deque<Dot *>*>::iterator it;
    std::list<std::deque<Dot *>*>::iterator end = m_chains.end();
    std::deque<Dot *> *chain;
    bool inserted = false;

    for (it = m_chains.begin(); !inserted && it != end; ++it) {
        chain = *it;

        if (chain->front() == &dot1) {
            chain->push_front(&dot2);
            inserted = true;
        }
        else if (chain->front() == &dot2) {
            chain->push_front(&dot1);
            inserted = true;
        }
        else if (chain->back() == &dot1) {
            chain->push_back(&dot2);
            inserted = true;
        }
        else if (chain->back() == &dot2) {
            chain->push_back(&dot1);
            inserted = true;
        }

        if (inserted) {
            return *chain;
        }
    }

    std::deque<Dot *> *newChain = new std::deque<Dot *>();
    newChain->push_back(&dot1);
    newChain->push_back(&dot2);

    m_chains.push_back(newChain);
    return *m_chains.back();
}

void GameEngine::dropFromChain(std::deque<Dot *> *&chain, Dot &dot1, Dot &dot2)
{
    std::deque<Dot *>::iterator it;
    std::deque<Dot *>::iterator begin = chain->begin();
    std::deque<Dot *>::iterator end = chain->end();
    std::deque<Dot *>::iterator next;
    Dot *firstDot = chain->front();
    Dot *lastDot = chain->back();
    Dot *currentDot;
    Dot *nextDot;
    std::deque<Dot *> *newChain;

    for (it = begin; it != end; ++it) {
        next = it + 1;

        if (next != end) {
            currentDot = *it;
            nextDot = *next;

            if ((currentDot == &dot1 && nextDot == &dot2)
                    || (currentDot == &dot2 && nextDot == &dot1)) {
                // break the chain at the current segment
                if (currentDot != firstDot && nextDot != lastDot) {
                    newChain = new std::deque<Dot *>(next, end);
                    m_chains.push_back(newChain);
                    chain->erase(next, end);
                }
                else {
                    if (currentDot == firstDot) {
                        chain->pop_front();
                    }
                    if (nextDot == lastDot) {
                        chain->pop_back();
                    }
                }

                // delete the chain if it has become empty
                if (chain->empty()) {
                    delete chain;
                    m_chains.remove(chain);
                }

                return;
            }
        }
    }
}

bool GameEngine::neighborsInChain(const std::deque<Dot *> &chain, const Dot &dot1, const Dot &dot2) const
{
    std::deque<Dot *>::const_iterator it;
    std::deque<Dot *>::const_iterator end = chain.end();
    std::deque<Dot *>::const_iterator next;

    for (it = chain.begin(); it != end; ++it) {
        next = it + 1;

        if (next != end) {
            if ((*it == &dot1 && *next == &dot2)
                    || (*it == &dot2 && *next == &dot1)) {
                return true;
            }
        }
    }

    return false;
}

void GameEngine::completeChain(const std::deque<Dot *> &chain)
{
    Dot *start = chain.front();
    Dot *end = chain.back();
    bool completed = false;
    bool surrounded = false;
    const std::deque<Dot *> *finalChain = &chain;
    std::deque<Dot *> closedChain;

    // check if the chain is already closed
    if (start == end) {
        completed = true;
        surrounded = true;
    }
    else {
        // try closing the chain
        if (closeChain(chain, closedChain)) {
            finalChain = &closedChain;
            completed = true;
            surrounded = true;
        }
        else {
            // check for edge-to-edge connection
            if (connectingEdges(chain)) {
                completed = true;
            }
        }
    }

    if (completed) {
        if (surrounded) {
            captureArea(*finalChain);
        }

        finalizeChain(*finalChain);
    }
}

bool GameEngine::closeChain(const std::deque<Dot *> &inChain, std::deque<Dot *> &outChain) const
{
    outChain = std::deque<Dot *>(inChain);

    if (inChain.size() < 2) {
        return false;
    }

    Dot *startDot = inChain.front();
    Dot *endDot = inChain.back();

    // check that the start and end dots are actually connected to something else
    if (findConnectedDots(*startDot).size() < 2 || findConnectedDots(*endDot).size() < 2) {
        return false;
    }

    /*
     * BEGIN: find path using an iterative DFS algorithm
     */
    std::stack<Dot *> unvisited;
    std::set<Dot *> visited;
    std::vector<Dot *> resultPath;
    bool pathFound = false;
    Dot *currentDot;
    Dot *nextDot;

    // mark all the dots in the chain as visited
    visited.insert(inChain.begin(), inChain.end());

    // push the start dot onto unvisited stack
    unvisited.push(startDot);

    // add the start dot to the path
    resultPath.push_back(startDot);

    while (!pathFound && !unvisited.empty()) {
        currentDot = unvisited.top();

        // find all dots connected to the current dot
        const std::deque<Dot *> &connectedDots = findConnectedDots(*currentDot);
        std::deque<Dot *>::const_iterator it;
        std::deque<Dot *>::const_iterator end = connectedDots.end();

        nextDot = 0;

        // find the next dot to visit
        for (it = connectedDots.begin(); it != end; ++it) {
            // check for the end dot
            if (*it == endDot && !neighborsInChain(inChain, *currentDot, *endDot)) {
                pathFound = true;
                break;
            }

            // check for a dot that has not been visited
            if (visited.find(*it) == visited.end()) {
                nextDot = *it;
                break;
            }
        }

        if (!pathFound) {
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
                visited.insert(nextDot);

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
        outChain.insert(outChain.end(), resultPath.rbegin(), resultPath.rend());
    }

    return pathFound;
}

bool GameEngine::connectingEdges(const std::deque<Dot *> &chain) const
{
    const Dot &start = *chain.front();
    const Dot &end = *chain.back();

    if (&start == &end) {
        return false;
    }
    else if (start.x() == end.x()) {
        return ((start.y() == 0 && end.y() == m_rows)
                || (end.y() == 0 && start.y() == m_rows));
    }
    else if (start.y() == end.y()) {
        return ((start.x() == 0 && end.x() == m_columns)
                || (end.x() == 0 && start.x() == m_columns));
    }
    else {
        return (start.x() == 0 || start.x() == m_columns
                || start.y() == 0 || start.y() == m_rows
                || end.x() == 0 || end.x() == m_columns
                || end.y() == 0 || end.y() == m_rows);
    }
}

void GameEngine::finalizeChain(const std::deque<Dot *> &chain)
{
    std::deque<Dot *>::const_iterator it;
    std::deque<Dot *>::const_iterator end = chain.end();
    std::deque<Dot *>::const_iterator next;
    std::deque<Dot *> *foundChain;

    for (it = chain.begin(); it != end; ++it) {
        next = it + 1;

        if (next != end) {
            Dot &dot1 = **it;
            Dot &dot2 = **next;

            if ((foundChain = findChain(dot1, dot2)) != 0) {
                dropFromChain(foundChain, dot1, dot2);
                m_lines.push_back(new Line(dot1, dot2));
            }
        }
    }
}

void GameEngine::captureArea(const std::deque<Dot *> &surroundingDots)
{
    std::deque<Dot *>::const_iterator it;
    std::deque<Dot *>::const_iterator end = surroundingDots.end();
    int minY = m_rows;
    int maxY = 0;
    std::map<int, int> leftBounds;
    std::map<int, int> rightBounds;

    for (it = surroundingDots.begin(); it != end; ++it) {
        const Dot &dot = **it;

        if (dot.y() < minY) {
            minY = dot.y();
        }
        if (dot.y() > maxY) {
            maxY = dot.y();
        }

        if (leftBounds.find(dot.y()) == leftBounds.end()) {
            leftBounds[dot.y()] = m_columns;
        }
        if (rightBounds.find(dot.y()) == rightBounds.end()) {
            rightBounds[dot.y()] = 0;
        }

        if (dot.x() < leftBounds[dot.y()]) {
            leftBounds[dot.y()] = dot.x();
        }
        if (dot.x() > rightBounds[dot.y()]) {
            rightBounds[dot.y()] = dot.x();
        }
    }

    int x;
    int y;
    Dot *dot;
    bool captured = false;

    for (y = minY + 1; y < maxY; ++y) {
        for (x = leftBounds[y] + 1; x < rightBounds[y]; ++x) {
            dot = findDot(m_dots, x, y);

            if (dot != 0) {
                if (dot->player() != m_currentPlayer && dot->isActive()) {
                    m_playerScores[m_currentPlayer] += 10;
                    dot->deactivate();
                    captured = true;
                }
            }
        }
    }

    if (captured) {
        emit playerScoresChanged();
    }
}

Dot *GameEngine::findDot(const std::deque<Dot *> &dots, int x, int y) const
{
    DotCoordinatesPredicate pred(x, y);
    std::deque<Dot *>::const_iterator it = std::find_if(dots.begin(), dots.end(), pred);

    if (it != dots.end()) {
        return *it;
    }
    else {
        return 0;
    }
}

Line *GameEngine::findLine(const Dot *endpoint1, const Dot *endpoint2) const
{
    const std::deque<Line *> &lines = m_lines;
    LineEndpointsPredicate pred(endpoint1, endpoint2);
    std::deque<Line *>::const_iterator it = std::find_if(lines.begin(), lines.end(), pred);

    if (it != lines.end()) {
        return *it;
    }
    else {
        return 0;
    }
}

std::deque<Line *> GameEngine::findLines(const Dot &endpoint) const
{
    std::deque<Line *> matches;
    const std::deque<Line *> &lines = m_lines;
    std::deque<Line *>::const_iterator it = lines.begin();
    std::deque<Line *>::const_iterator end = lines.end();
    LineEndpointsPredicate pred(&endpoint);

    while (it != end) {
        it = std::find_if(it, end, pred);

        if (it != end) {
            matches.push_back(*it);
            it++; // start checking from next element
        }
    }

    return matches;
}

std::deque<Dot *> *GameEngine::findChain(const Dot &dot1, const Dot &dot2) const
{
    const std::list<std::deque<Dot *>*> &chains = m_chains;
    std::list<std::deque<Dot *>*>::const_iterator chains_it;
    std::list<std::deque<Dot *>*>::const_iterator chains_end = chains.end();
    std::deque<Dot *> *chain;

    for (chains_it = chains.begin(); chains_it != chains_end; ++chains_it) {
        chain = *chains_it;

        if (neighborsInChain(*chain, dot1, dot2)) {
            return chain;
        }
    }

    return 0;
}

std::deque<Dot *> GameEngine::findConnectedDots(const Dot &dot) const
{
    std::deque<Dot *> connectedDots;

    // find connected dots in the existing lines
    {
        const std::deque<Line *> &lines = findLines(dot);
        std::deque<Line *>::const_iterator lines_it;
        std::deque<Line *>::const_iterator lines_end = lines.end();

        for (lines_it = lines.begin(); lines_it != lines_end; ++lines_it) {
            const Line &line = **lines_it;

            if (&line.endpoint1() != &dot) {
                connectedDots.push_back(&line.endpoint1());
            }
            else {
                connectedDots.push_back(&line.endpoint2());
            }
        }
    }

    // find connected dots in all chains
    {
        const std::list<std::deque<Dot *>*> &chains = m_chains;
        std::list<std::deque<Dot *>*>::const_iterator chains_it;
        std::list<std::deque<Dot *>*>::const_iterator chains_end = chains.end();
        std::deque<Dot *> *chain;
        std::deque<Dot *>::const_iterator it;
        std::deque<Dot *>::const_iterator begin;
        std::deque<Dot *>::const_iterator end;
        bool found;

        for (chains_it = chains.begin(); chains_it != chains_end; ++chains_it) {
            chain = *chains_it;
            begin = chain->begin();
            end = chain->end();
            found = false;

            for (it = begin; !found && it != end; ++it) {
                if (*it == &dot) {
                    if (it - 1 != begin - 1) {
                        connectedDots.push_back(*(it - 1));
                    }
                    if (it + 1 != end) {
                        connectedDots.push_back(*(it + 1));
                    }
                    found = true;
                }
            }
        }
    }

    return connectedDots;
}

void GameEngine::clearTurnData()
{
    std::list<std::deque<Dot *>*>::iterator it;
    std::list<std::deque<Dot *>*>::iterator end = m_chains.end();

    for (it = m_chains.begin(); it != end; ++it) {
        delete *it;
    }
    m_chains.clear();
}

void GameEngine::clearGameData()
{
    std::deque<Dot *>::iterator dots_it;
    std::deque<Dot *>::iterator dots_end = m_dots.end();

    for (dots_it = m_dots.begin(); dots_it != dots_end; ++dots_it) {
        delete *dots_it;
    }
    m_dots.clear();

    std::deque<Line *>::iterator lines_it;
    std::deque<Line *>::iterator lines_end = m_lines.end();

    for (lines_it = m_lines.begin(); lines_it != lines_end; ++lines_it) {
        delete *lines_it;
    }
    m_lines.clear();
}
