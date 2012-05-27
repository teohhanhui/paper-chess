#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QObject>
#include <vector>
#include "dot.h"
#include "line.h"

class GameEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int rows READ rows WRITE setRows)
    Q_PROPERTY(int columns READ columns WRITE setColumns)
    Q_PROPERTY(int turnLimit READ turnLimit WRITE setTurnLimit)

public:
    explicit GameEngine(QObject *parent = 0);
    ~GameEngine();

    enum Stage {PlaceDotStage, ConnectingStage, EndStage};

    int rows() const;
    void setRows(int rows);

    int columns() const;
    void setColumns(int columns);

    int turnLimit() const;
    void setTurnLimit(int turnLimit);

public slots:
    Q_INVOKABLE bool placeDot(int x, int y);
    Q_INVOKABLE bool connectDots(int x1, int y1, int x2, int y2);

signals:
    void chainCompleted();

private:
    /* iterates through surround chain & check dot status & dot player
     * returns 1 for surround if able, 0 if fail
     */
    void checkChain();   

    /* Closes the chain using existing lines.
     * Returns true if the chain is successfully closed, false otherwise.
     */
    bool closeGap(std::vector<Dot *> chain) const;

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
    void captureArea(const std::vector<Dot *> surroundingDots);

    void captureDot(int x, int y);

    /* Finds a dot within the given vector with coordinates (x,y).
     * Returns a pointer to the dot if found, a null pointer otherwise.
     */
    Dot *findDot(const std::vector<Dot *> dots, int x, int y) const;

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
    std::vector<Dot *> m_chain;
    std::vector<Line *> m_lines;
    int m_currentPlayer;
    Stage m_stage;
};

#endif // GAMEENGINE_H
