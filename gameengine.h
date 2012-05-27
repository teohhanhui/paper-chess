#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QObject>
#include <vector>
#include <deque>

class Dot;
class Line;

class GameEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int rows READ rows)
    Q_PROPERTY(int columns READ columns)
    Q_PROPERTY(int turnLimit READ turnLimit)

public:
    explicit GameEngine(QObject *parent = 0);
    ~GameEngine();

    enum Stage {PlaceDotStage, ConnectingStage, EndStage};

    int rows() const;
    int columns() const;
    int turnLimit() const;

public slots:
    void newGame(int rows, int columns, int turnLimit);
    bool placeDot(int x, int y);
    bool connectDots(int x1, int y1, int x2, int y2);

signals:
    void chainCompleted();

private:
    /* Checks if the point at the specified coordinates is active.
     * Returns true if the point is active, false otherwise.
     */
    bool isPointActive(int x, int y) const;

    /* Checks if any chain contains the specified dots.
     * Returns true if the dots belong in any of the chains, false otherwise.
     */
    bool inChain(Dot *dot1, Dot *dot2) const;

    /* Inserts the specified dots into a suitable chain.
     * A new chain is created if no existing chain has one of the dots as an endpoint.
     * Returns the chain where the dots were inserted.
     */
    std::deque<Dot *> &insertIntoChain(Dot *dot1, Dot *dot2);

    /* Checks for a closed shape in which the chain is a part of.
     * All existing lines and all chains may be used to make the connection.
     */
    void checkChain(std::deque<Dot *> &chain);

    /* Closes the chain using existing lines.
     * Returns true if the chain is successfully closed, false otherwise.
     */
    bool closeGap(std::deque<Dot *> chain) const;

    bool isOnEdge(const Dot *dot) const;

    void linkChain();

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
    void captureArea(const std::deque<Dot *> surroundingDots);

    void captureDot(int x, int y);

    /* Finds a dot within the given container with coordinates (x,y).
     * Returns a pointer to the dot if found, a null pointer otherwise.
     */
    template <typename Container>
    Dot *findDot(const Container &dots, int x, int y) const;

    /* Finds an existing line with the specified endpoints.
     * Returns a pointer to the line if found, a null pointer otherwise.
     */
    Line *findLine(const Dot *endpoint1, const Dot *endpoint2 = 0) const;

    /* Finds all existing lines containing the specified endpoint.
     * Returns a vector of the lines found.
     */
    std::vector<Line *> findLines(const Dot *endpoint) const;

    /* Finds all dots connected to the specified dot.
     * Returns a vector of the dots found.
     */
    std::vector<Dot *> findConnectedDots(const Dot *dot) const;

    int m_rows;
    int m_columns;
    int m_turnLimit;
    std::vector<Dot *> m_dots;
    std::vector<Line *> m_lines;
    std::vector<Dot *> m_chain;
    std::vector<std::deque<Dot *> > m_chains;
    std::vector<bool> m_pointActive;
    int m_currentPlayer;
    Stage m_stage;
};

#endif // GAMEENGINE_H
