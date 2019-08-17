#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QBitArray>
#include <QObject>
#include <QVarLengthArray>
#include <QVariantList>
#include <deque>
#include <list>
#include <vector>

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
    Q_PROPERTY(QVariantList playerNames READ playerNames WRITE setPlayerNames
                   NOTIFY playerNamesChanged)
    Q_PROPERTY(
        QVariantList playerScores READ playerScores NOTIFY playerScoresChanged)
    Q_ENUMS(Stage)

public:
    explicit GameEngine(QObject *parent = nullptr);
    ~GameEngine();

    enum Stage
    {
        PlaceDotStage,
        ConnectDotsStage,
        EndStage
    };

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

    /// Gets the dot with the specified coordinates.
    ///
    /// \returns a pointer to the dot if found, a null pointer otherwise.
    const Dot *getDotAt(int x, int y) const;

    /// Gets a list of all dots.
    ///
    /// \returns a list which is a snapshot of all the dots.
    std::vector<const Dot *> getDots() const;

    /// Gets a list of all lines.
    ///
    /// \returns a list which is a snapshot of all the lines.
    std::vector<const Line *> getLines(int player) const;

    /// Gets a list of all the chains.
    ///
    /// \returns a list of lists which is a snapshot of all the chains.
    std::vector<std::vector<const Dot *>> getChains() const;

    /// Checks if a dot can be placed at the specified coordinates.
    ///
    /// \returns true if the dot can be placed, false otherwise.
    bool canPlaceDot(int x, int y) const;

    /// Checks if the specified dots can be connected.
    ///
    /// \returns true if the dots can be connected, false otherwise.
    bool canConnectDots(const Dot &dot1, const Dot &dot2) const;

    /// Checks if the two specified dots are connected in the specified chain.
    ///
    /// \returns true if the two dots are connected, false otherwise.
    template <typename InputIterator>
    bool neighborsInChain(
        InputIterator chainStart,
        InputIterator chainEnd,
        const Dot &dot1,
        const Dot &dot2) const;

public slots:
    void newGame(int rows, int columns, int turnLimit);
    bool placeDot(int x, int y);
    bool connectDots(int x1, int y1, int x2, int y2);
    void connectAllDots();
    void endTurn();

signals:
    void playerNamesChanged();
    void gameStarted();
    void turnsLeftChanged();
    void currentPlayerChanged();
    void stageChanged();
    void dotsChanged();
    void chainsChanged();
    void linesChanged();
    void playerScoresChanged();
    void turnEnded();

private:
    /// Checks if the point at the specified coordinates is active.
    ///
    /// \returns true if the point is active, false otherwise.
    bool isPointActive(int x, int y) const;

    /// Deactivates the point at the specified coordinates.
    void deactivatePoint(int x, int y);

    /// Checks if the dots are connected in any chain (i.e. the dots are
    /// side-by-side).
    ///
    /// \returns true if the chain is found, false otherwise.
    bool connectedInChain(const Dot &dot1, const Dot &dot2) const;

    /// Inserts the specified dots at the beginning or end of a chain.
    ///
    /// A new chain is created if the dots cannot be inserted into an existing
    /// chain.
    ///
    /// \returns the chain where the dots were inserted.
    std::deque<Dot *> &addToChains(Dot &dot1, Dot &dot2);

    /// Splits the specified chain at the segment between the two specified
    /// dots.
    ///
    /// If the segment is at the middle of the chain, a new chain is added.
    /// The original chain will be removed if it becomes empty after the split.
    void cutChain(std::deque<Dot *> *&chain, Dot &dot1, Dot &dot2);

    /// Completes the specified chain.
    ///
    /// If successful, all the necessary followup actions will be performed.
    template <typename InputIterator>
    void completeChain(InputIterator chainStart, InputIterator chainEnd);

    /// Closes the chain using connections from existing lines and all the
    /// chains. The resultant chain is stored into outChain.
    ///
    /// \returns true if the chain is successfully closed, false otherwise.
    template <typename InputIterator>
    bool closeChain(
        InputIterator chainStart,
        InputIterator chainEnd,
        std::deque<Dot *> &outChain) const;

    /// Forms a barricade off the grid's borders.
    ///
    /// The traversal follows existing lines and all other chains.
    ///
    /// \returns true if a barricade can be formed, false otherwise.
    template <typename InputIterator>
    bool formBarricade(InputIterator chainStart, InputIterator chainEnd) const;

    /// Extends the chain to the grid's borders using existing lines and all the
    /// chains.
    ///
    /// \returns true if the chain can be extended to go from border to border,
    /// false otherwise.
    template <typename InputIterator>
    bool extendToBorders(
        InputIterator chainStart,
        InputIterator chainEnd,
        std::deque<Dot *> &outChain) const;

    /// Add all line segments from the input chain.
    ///
    /// An added line segment is removed from its original chain. A chain is
    /// removed when it becomes empty.
    template <typename InputIterator>
    void finalizeChain(InputIterator chainStart, InputIterator chainEnd);

    /// Capture dots in the area enclosed by the specified surrounding dots.
    ///
    /// A dot can be captured if it belong to another player and has not been
    /// previously captured. The current player's score is incremented for each
    /// dot captured.
    template <typename InputIterator>
    void captureArea(InputIterator chainStart, InputIterator chainEnd);

    /// Finds a dot within the given list with coordinates (x,y).
    ///
    /// \returns a pointer to the dot if found, a null pointer otherwise.
    Dot *findDot(const std::deque<Dot *> &dots, int x, int y) const;

    /// Finds an existing line with the specified endpoints.
    ///
    /// \returns a pointer to the line if found, a null pointer otherwise.
    Line *findLine(const Dot *endpoint1, const Dot *endpoint2 = nullptr) const;

    /// Finds all existing lines containing the specified endpoint.
    ///
    /// \returns a list of the lines found.
    std::deque<Line *> findLines(const Dot &endpoint) const;

    /// Finds an existing chain where the two specified dots are connected.
    ///
    /// \returns a pointer to the chain if found, a null pointer otherwise.
    std::deque<Dot *> *findChain(const Dot &dot1, const Dot &dot2) const;

    /// Finds all dots connected to the specified dot.
    ///
    /// \returns a list of the dots found.
    std::deque<Dot *> findConnectedDots(const Dot &dot) const;

    /// Finds a path from the start of the chain to a dot for which the
    /// predicate is true.
    ///
    /// The dots (chainStart, chainEnd] are excluded during the traversal. The
    /// current path is stored in resultPath.
    ///
    /// \returns true if a path is found, false otherwise.
    template <typename InputIterator, typename Predicate, typename Container>
    bool findPath(
        InputIterator chainStart,
        InputIterator chainEnd,
        Predicate pred,
        Container &resultPath) const;

    /// Clears all data pertaining to the turn.
    void clearTurnData();

    /// Clears all data pertaining to the game.
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
    std::list<std::deque<Dot *> *> m_chains;
};

#endif // GAMEENGINE_H
