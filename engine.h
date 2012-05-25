#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include <vector>
#include "dot.h"
#include "line.h"

class GameEngine : public QObject {
public:
    enum Stage {PlaceDotStage, ConnectingStage, EndStage};

    explicit GameEngine(QObject *parent = 0);
    ~GameEngine();
    bool placeDot(int x, int y);
    bool connectDots(int x1, int y1, int x2, int y2);

private:

    /* iterates through surround chain & check dot status & dot player
     * returns 1 for surround if able, 0 if fail
     */
    bool chainCheck();

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
    int captureDot(const std::vector<Dot *> surroundingDots, int capturedPlayer);

    Dot *searchDot(const std::vector<Dot *> dots, int x, int y) const;

    void eatDot(int x, int y);

    std::vector<Dot *> m_dots;
    std::vector<Dot *> m_chain;
    std::vector<Line *> m_lines;
    int m_currentPlayer;
    Stage m_stage;
};

#endif // ENGINE_H
