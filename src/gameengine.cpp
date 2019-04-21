#include "gameengine.h"
#include <algorithm>
#include <stack>
#include <set>
#include "dot.h"
#include "line.h"
#include "dotcoordinatespredicate.h"
#include "lineendpointspredicate.h"
#include "dotonborderpredicate.h"

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

std::vector<const Dot *> GameEngine::getDots() const
{
    std::vector<const Dot *> outDots;
    const std::deque<Dot *> &dots = m_dots;
    std::deque<Dot *>::const_iterator it;
    std::deque<Dot *>::const_iterator end = dots.end();

    for (it = dots.begin(); it != end; ++it) {
        outDots.push_back(*it);
    }

    return outDots;
}

std::vector<const Line *> GameEngine::getLines(int player) const
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

std::vector<std::vector<const Dot *> > GameEngine::getChains() const
{
    std::vector<std::vector<const Dot *> > outChains;
    const std::list<std::deque<Dot *>*> &chains = m_chains;
    std::list<std::deque<Dot *>*>::const_iterator chains_it;
    std::list<std::deque<Dot *>*>::const_iterator chains_end = chains.end();
    std::deque<Dot *>::const_iterator it;
    std::deque<Dot *>::const_iterator end;

    for (chains_it = chains.begin(); chains_it != chains_end; ++chains_it) {
        std::vector<const Dot *> outChain;
        const std::deque<Dot *> &chain = **chains_it;
        end = chain.end();

        for (it = chain.begin(); it != end; ++it) {
            outChain.push_back(*it);
        }

        outChains.push_back(outChain);
    }

    return outChains;
}

bool GameEngine::canPlaceDot(int x, int y) const
{
    return (isPointActive(x, y) && findDot(m_dots, x, y) == nullptr);
}

bool GameEngine::canConnectDots(const Dot &dot1, const Dot &dot2) const
{
    // check if the dots are the same dot
    if (&dot1 == &dot2 || dot1 == dot2) {
        return false;
    }

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
            || findLine(&dot1, &dot2) != nullptr
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

        if (blockingDot1 == nullptr || blockingDot2 == nullptr) {
            return true;
        }

        const Line *blockingLine = findLine(blockingDot1, blockingDot2);

        if (blockingLine == nullptr) {
            return true;
        }
    }

    return false;
}

template <typename InputIterator>
bool GameEngine::neighborsInChain(InputIterator chainStart, InputIterator chainEnd,
                                  const Dot &dot1, const Dot &dot2) const
{
    InputIterator it;
    Dot *currentDot;
    Dot *nextDot;

    for (it = chainStart; it != chainEnd; ++it) {
        currentDot = *it;
        nextDot = *(it + 1);

        if ((*currentDot == dot1 && *nextDot == dot2)
                || (*currentDot == dot2 && *nextDot == dot1)) {
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

    Dot *dot1 = findDot(m_dots, x1, y1);
    Dot *dot2 = findDot(m_dots, x2, y2);

    // check if the dots exist
    if (dot1 == nullptr || dot2 == nullptr) {
        return false;
    }

    if (!canConnectDots(*dot1, *dot2)) {
        return false;
    }

    std::deque<Dot *> chain = addToChains(*dot1, *dot2);
    std::deque<Dot *>::iterator first = chain.begin();
    std::deque<Dot *>::iterator last = chain.end() - 1;
    std::deque<Dot *>::iterator head_it;
    std::deque<Dot *>::iterator tail_it;

    for (head_it = first; head_it != last; ++head_it) {
        for (tail_it = last; tail_it != head_it; --tail_it) {
            completeChain(head_it, tail_it);
        }
    }

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

void GameEngine::deactivatePoint(int x, int y)
{
    m_pointDisabled[y * (m_columns + 1) + x] = true;
}

bool GameEngine::connectedInChain(const Dot &dot1, const Dot &dot2) const
{
    return (findChain(dot1, dot2) != nullptr);
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

void GameEngine::cutChain(std::deque<Dot *> *&chain, Dot &dot1, Dot &dot2)
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

template <typename InputIterator>
void GameEngine::completeChain(InputIterator chainStart, InputIterator chainEnd)
{
    bool completed = false;
    bool surrounded = false;
    std::deque<Dot *> closedChain;

    // check if the chain is already closed
    if (*chainStart == *chainEnd) {
        completed = true;
        surrounded = true;
    }
    else {
        // try closing the chain
        if (closeChain(chainStart, chainEnd, closedChain)) {
            chainStart = closedChain.begin();
            chainEnd = closedChain.end() - 1;
            completed = true;
            surrounded = true;
        }
        else {
            // try forming a barricade
            if (formBarricade(chainStart, chainEnd)) {
                completed = true;
            }
        }
    }

    if (completed) {
        if (surrounded) {
            captureArea(chainStart, chainEnd);
        }

        finalizeChain(chainStart, chainEnd);
    }
}

template <typename InputIterator>
bool GameEngine::closeChain(InputIterator chainStart, InputIterator chainEnd, std::deque<Dot *> &outChain) const
{
    // initialize the output chain to contain all dots from the input chain
    outChain.clear();
    outChain.insert(outChain.end(), chainStart, chainEnd + 1);

    // check if there are at least 2 dots in the chain
    if (chainStart + 1 == chainEnd) {
        return false;
    }

    Dot *startDot = *chainStart;
    Dot *endDot = *chainEnd;

    // check that the start and end dots are actually connected to something else
    if (findConnectedDots(*startDot).size() < 2 || findConnectedDots(*endDot).size() < 2) {
        return false;
    }

    bool pathFound;
    std::vector<Dot *> resultPath;
    pathFound = findPath(chainStart, chainEnd,
                         DotCoordinatesPredicate(endDot->x(), endDot->y()), resultPath);

    if (pathFound) {
        // add dots from the result path to close the chain
        outChain.insert(outChain.end(), resultPath.rbegin() + 1, resultPath.rend());
    }

    return pathFound;
}

template <typename InputIterator>
bool GameEngine::formBarricade(InputIterator chainStart, InputIterator chainEnd) const
{
    std::deque<Dot *> extendedChain;

    // try to extend the chain to the borders
    if (!extendToBorders(chainStart, chainEnd, extendedChain)) {
        return false;
    }

    std::deque<Dot *>::const_iterator it;
    std::deque<Dot *>::const_iterator end = extendedChain.end();
    int minX = m_columns;
    int minY = m_rows;
    int maxX = 0;
    int maxY = 0;

    for (it = extendedChain.begin(); it != end; ++it) {
        const Dot &dot = **it;

        // check if the dots form a valid chain
        if ((it + 1) != end && !dot.isNeighbor(**(it + 1))) {
            return false;
        }

        if (dot.x() < minX) {
            minX = dot.x();
        }
        if (dot.x() > maxX) {
            maxX = dot.x();
        }
        if (dot.y() < minY) {
            minY = dot.y();
        }
        if (dot.y() > maxY) {
            maxY = dot.y();
        }
    }

    // a barricade MUST either:
    // seal off some space
    if ((maxX - minX) > 0 && (maxY - minY) > 0) {
        return true;
    }
    else { // OR
        const Dot &startDot = *extendedChain.front();
        const Dot &endDot = *extendedChain.back();

        // cut across the entire horizontal or vertical span
        if ((startDot.x() == 0 && endDot.x() == m_columns)
             || (startDot.x() == m_columns && endDot.x() == 0)
             || (startDot.y() == 0 && endDot.y() == m_rows)
             || (startDot.y() == m_rows && endDot.y() == 0)) {
            return true;
        }
    }

    return false;
}

template <typename InputIterator>
bool GameEngine::extendToBorders(InputIterator chainStart, InputIterator chainEnd, std::deque<Dot *> &outChain) const
{
    DotOnBorderPredicate pred(0, m_columns, 0, m_rows);
    std::vector<Dot *> resultPath;
    bool startOnBorder = pred(*chainStart);
    bool endOnBorder = pred(*chainEnd);

    if (!startOnBorder) {
        startOnBorder = findPath(chainStart, chainEnd, pred, resultPath);

        // prepend dots before the start dot
        if (startOnBorder) {
            outChain.insert(outChain.end(), resultPath.rbegin(), resultPath.rend() - 1);
        }
    }

    // copy all dots from the input chain to the output chain
    outChain.insert(outChain.end(), chainStart, chainEnd + 1);

    if (!endOnBorder) {
        std::vector<Dot *> inChain;
        inChain.insert(inChain.end(), chainStart, chainEnd + 1);

        endOnBorder = findPath(inChain.rbegin(), inChain.rend() - 1, pred, resultPath);

        // append dots after the end dot
        if (endOnBorder) {
            outChain.insert(outChain.end(), resultPath.begin() + 1, resultPath.end());
        }
    }

    return (startOnBorder && endOnBorder);
}

template <typename InputIterator>
void GameEngine::finalizeChain(InputIterator chainStart, InputIterator chainEnd)
{
    InputIterator it;
    InputIterator next;
    std::deque<Dot *> *foundChain;

    for (it = chainStart; it != chainEnd + 1; ++it) {
        next = it + 1;

        if (next != chainEnd + 1) {
            Dot &dot1 = **it;
            Dot &dot2 = **next;

            if ((foundChain = findChain(dot1, dot2)) != nullptr) {
                cutChain(foundChain, dot1, dot2);
                m_lines.push_back(new Line(dot1, dot2));
            }
        }
    }
}

template <typename InputIterator>
void GameEngine::captureArea(InputIterator chainStart, InputIterator chainEnd)
{
    InputIterator it;
    int minY = m_rows;
    int maxY = 0;
    std::map<int, int> leftBounds;
    std::map<int, int> rightBounds;

    for (it = chainStart; it != chainEnd + 1; ++it) {
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

            if (dot != nullptr) {
                if (dot->player() != m_currentPlayer && dot->isActive()) {
                    m_playerScores[m_currentPlayer] += 10;
                    dot->deactivate();
                    captured = true;
                }
            }

            deactivatePoint(x, y);
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
        return nullptr;
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
        return nullptr;
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

        if (neighborsInChain(chain->begin(), chain->end() - 1, dot1, dot2)) {
            return chain;
        }
    }

    return nullptr;
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

template <typename InputIterator, typename Predicate, typename Container>
bool GameEngine::findPath(InputIterator chainStart, InputIterator chainEnd,
                          Predicate pred, Container &resultPath) const
{
    /*
     * Implementation note: Iterative DFS algorithm
     */

    std::stack<Dot *> unvisited;
    std::set<Dot *> visited;
    Dot *startDot = *chainStart;
    Dot *currentDot;
    Dot *nextDot;

    // mark all the dots in the chain as visited
    visited.insert(chainStart, chainEnd + 1);

    // push the start dot onto unvisited stack
    unvisited.push(startDot);

    // clear the result path
    resultPath.clear();

    // add the start dot to the result path
    resultPath.push_back(startDot);

    while (!unvisited.empty()) {
        currentDot = unvisited.top();

        // find all dots connected to the current dot
        const std::deque<Dot *> &connectedDots = findConnectedDots(*currentDot);
        std::deque<Dot *>::const_iterator it;
        std::deque<Dot *>::const_iterator end = connectedDots.end();

        nextDot = nullptr;

        // find the next dot to visit
        for (it = connectedDots.begin(); it != end; ++it) {
            // check terminating condition
            if (pred(*it) && !neighborsInChain(chainStart, chainEnd, *currentDot, **it)) {
                resultPath.push_back(*it);
                return true;
            }

            // check for a dot that has not been visited
            if (visited.find(*it) == visited.end()) {
                nextDot = *it;
                break;
            }
        }

        // check for dead end
        if (nextDot == nullptr) {
            // pop the current dot off the unvisited stack
            unvisited.pop();

            // remove the current dot from the result path
            resultPath.pop_back();
        }
        else {
            // push the dot onto unvisited stack
            unvisited.push(nextDot);

            // mark the dot as visited
            visited.insert(nextDot);

            // add the dot to the result path
            resultPath.push_back(nextDot);
        }
    }

    return false;
}

void GameEngine::clearTurnData()
{
    for (const std::deque<Dot *> *chain : m_chains) {
        delete chain;
    }
    m_chains.clear();
}

void GameEngine::clearGameData()
{
    for (const Dot *dot : m_dots) {
        delete dot;
    }
    m_dots.clear();

    for (const Line *line : m_lines) {
        delete line;
    }
    m_lines.clear();
}
