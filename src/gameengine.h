#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QObject>
#include <QBitArray>

class Dot;
class Line;

class GameEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int rows READ rows)
    Q_PROPERTY(int columns READ columns)
    Q_PROPERTY(int turnLimit READ turnLimit)
    Q_PROPERTY(int turnsLeft READ turnsLeft NOTIFY turnsLeftChanged)
    Q_PROPERTY(int currentPlayer READ currentPlayer NOTIFY currentPlayerChanged)
    Q_PROPERTY(Stage stage READ stage NOTIFY stageChanged)
    Q_ENUMS(Stage)

public:
    explicit GameEngine(QObject *parent = 0);
    ~GameEngine();

    enum Stage { PlaceDotStage, ConnectDotsStage, EndStage };

    int rows() const;
    int columns() const;
    int turnLimit() const;
    int turnsLeft() const;
    int currentPlayer() const;
    Stage stage() const;

    const Dot *getDotAt(int x, int y) const;
    const QList<Dot *> &getDots() const;
    const QList<Line *> &getLines() const;

    bool canPlaceDot(int x, int y) const;
    bool canConnectDots(int x1, int y1, int x2, int y2) const;

signals:
    void gameStarted();
    void chainCompleted();
    void turnsLeftChanged();
    void currentPlayerChanged();
    void stageChanged();

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

    /* Checks if any chain contains the specified dots.
     * Returns true if the dots belong in any of the chains, false otherwise.
     */
    bool connectedInChain(Dot *dot1, Dot *dot2) const;

    /* Inserts the specified dots into a suitable chain.
     * A new chain is created if no existing chain has one of the dots as an endpoint.
     * Returns the chain where the dots were inserted.
     */
    QList<Dot *> &insertIntoChain(Dot *dot1, Dot *dot2);

    /* Checks for a closed shape in which the chain is a part of.
     * All existing lines and all chains may be used to make the connection.
     */
    void checkChain(QList<Dot *> &chain);

    /* Closes the chain using existing lines.
     * Returns true if the chain is successfully closed, false otherwise.
     */
    bool closeGap(QList<Dot *> chain) const;

    bool isOnEdge(const Dot *dot) const;

    void linkChain(const QList<Dot *> &chain);

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
    void captureArea(const QList<Dot *> surroundingDots);

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
     * Returns a list of the lines found.
     */
    QList<Line *> findLines(const Dot *endpoint) const;

    /* Finds all dots connected to the specified dot.
     * Returns a list of the dots found.
     */
    QList<Dot *> findConnectedDots(const Dot *dot) const;

    int m_rows;
    int m_columns;
    int m_turnLimit;
    int m_turn;
    int m_currentPlayer;
    Stage m_stage;
    QBitArray m_pointDisabled;
    QList<Dot *> m_dots;
    QList<Line *> m_lines;
    QList<QList<Dot *> > m_chains;
};

#endif // GAMEENGINE_H
