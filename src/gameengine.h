#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QObject>
#include <QVariantList>
#include <QVarLengthArray>
#include <QBitArray>
#include <deque>
#include <vector>
#include <list>

class Dot;
class Line;

class GameEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int numPlayers READ numPlayers)
    Q_PROPERTY(int rows READ rows)
    Q_PROPERTY(int columns READ columns)
    Q_PROPERTY(int turnLimit READ turnLimit)
    Q_PROPERTY(int turnsLeft READ turnsLeft NOTIFY turnsLeftChanged)
    Q_PROPERTY(int currentPlayer READ currentPlayer NOTIFY currentPlayerChanged)
    Q_PROPERTY(Stage stage READ stage NOTIFY stageChanged)
    Q_PROPERTY(QVariantList playerNames READ playerNames WRITE setPlayerNames NOTIFY playerNamesChanged)
    Q_PROPERTY(QVariantList playerScores READ playerScores NOTIFY playerScoresChanged)
    Q_ENUMS(Stage)

public:
    explicit GameEngine(QObject *parent = 0);
    ~GameEngine();

    enum Stage { PlaceDotStage, ConnectDotsStage, EndStage };

    int numPlayers() const;

    int rows() const;

    int columns() const;

    int turnLimit() const;

    int turnsLeft() const;

    int currentPlayer() const;

    Stage stage() const;

    QVariantList playerNames() const;
    void setPlayerNames(QVariantList &list);

    QVariantList playerScores() const;

    const Dot *getDotAt(int x, int y) const;
    const std::deque<Dot *> &getDots() const;
    const std::vector<const Line *> getLines(int player) const;
    const std::vector<const std::deque<Dot *>*> getChains() const;

    bool canPlaceDot(int x, int y) const;
    bool canConnectDots(int x1, int y1, int x2, int y2) const;

signals:
    void playerNamesChanged();
    void gameStarted();
    void turnsLeftChanged();
    void currentPlayerChanged();
    void stageChanged();
    void chainsChanged();
    void playerScoresChanged();
    void turnEnded();

public slots:
    void newGame(int rows, int columns, int turnLimit);
    bool placeDot(int x, int y);
    bool connectDots(int x1, int y1, int x2, int y2);
    void endTurn();

private:
    /* Checks if the point at the specified coordinates is active.
     * Returns true if the point is active, false otherwise.
     */
    bool isPointActive(int x, int y) const;

    /* Checks if the dots are connected in any chain (i.e. the dots are side-by-side).
     * Returns true if the chain is found, false otherwise.
     */
    bool connectedInChain(const Dot &dot1, const Dot &dot2) const;

    /* Inserts the specified dots at the beginning or end of a chain.
     * A new chain is created if the dots cannot be inserted into an existing chain.
     * Returns the chain where the dots were inserted.
     */
    std::deque<Dot *> &addToChains(Dot &dot1, Dot &dot2);

    void dropFromChain(std::deque<Dot *> *&chain, Dot &dot1, Dot &dot2);

    bool neighborsInChain(const std::deque<Dot *> &chain, const Dot &dot1, const Dot &dot2) const;

    /* Completes the specified chain.
     * If successful, all the necessary followup actions will be performed.
     */
    void completeChain(const std::deque<Dot *> &chain);

    /* Closes the chain using connections from existing lines and all the chains.
     * The resultant chain is stored into outChain.
     * Returns true if the chain is successfully closed, false otherwise.
     */
    bool closeChain(const std::deque<Dot *> &inChain, std::deque<Dot *> &outChain) const;

    /* Checks if the chain connects one edge of the grid to another.
     * Returns true if the chain connects edges, false otherwise.
     */
    bool connectingEdges(const std::deque<Dot *> &chain) const;

    /* Add all line segments from the input chain.
     * An added line segment is removed from its original chain.
     * A chain is removed when it becomes empty.
     */
    void finalizeChain(const std::deque<Dot *> &chain);

    /* check number of captured dots, returns number of dots captured
     * idea of calculating captured dots: with limits of MAX x, MAX y, MIN x, MIN y
     * we can visualize the captured dots in a shape.  based on the coordinates,
     * the captured dots will be calculated line by line.
     *
     * Steps:
     * 1. based on vector input which lists the dots for a complete surround, find dots with the same y-axis.
     * 2. this becomes the MIN & MAX values for x for the said row, in which we check all existing dots in between.
     * 3. increment counter if captured a dot.
     * 4. repeat from MIN y to MAX y
     */
    void captureArea(const std::deque<Dot *> &surroundingDots);

    /* Finds a dot within the given list with coordinates (x,y).
     * Returns a pointer to the dot if found, a null pointer otherwise.
     */
    Dot *findDot(const std::deque<Dot *> &dots, int x, int y) const;

    /* Finds an existing line with the specified endpoints.
     * Returns a pointer to the line if found, a null pointer otherwise.
     */
    Line *findLine(const Dot *endpoint1, const Dot *endpoint2 = 0) const;

    /* Finds all existing lines containing the specified endpoint.
     * Returns a list of the lines found.
     */
    std::deque<Line *> findLines(const Dot &endpoint) const;

    std::deque<Dot *> *findChain(const Dot &dot1, const Dot &dot2) const;

    /* Finds all dots connected to the specified dot.
     * Returns a list of the dots found.
     */
    std::deque<Dot *> findConnectedDots(const Dot &dot) const;

    void clearTurnData();

    void clearGameData();

    static const int DEFAULT_NUM_PLAYERS = 2;

    const int m_numPlayers;

    int m_rows;
    int m_columns;
    int m_turnLimit;
    int m_turnsLeft;
    int m_currentPlayer;
    Stage m_stage;
    QVarLengthArray<QString, DEFAULT_NUM_PLAYERS> m_playerNames;
    QVarLengthArray<int, DEFAULT_NUM_PLAYERS> m_playerScores;
    QBitArray m_pointDisabled;
    std::deque<Dot *> m_dots;
    std::deque<Line *> m_lines;
    std::list<std::deque<Dot *>*> m_chains;
};

#endif // GAMEENGINE_H
