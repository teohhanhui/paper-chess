#ifndef ENGINE_H
#define ENGINE_H
#include "dot.h"
#include <QObject>
#include <vector>
using namespace std;

class Engine : public QObject{
public:
    enum Stage{PlaceDotStage, ConnectingStage, EndStage};

    explicit Engine(QObject *parent = 0);

    bool placeDot(int x, int y);

    bool connectDots(int x1, int y1, int x2, int y2);
private:


    /*iterates through surround chain & check dot status & dot player
    returns 1 for surround if able, 0 if fail*/
    bool chainCheck();

    /*check number of captured dots, returns number of dots captured
    idea of calculating captured dots: with limits of MAX x, MAX y, MIN x, MIN y
    we can visualize the captured dots in a shape.  based on the coordinates,
    the captured dots will be calculated line by line.
    Steps: based on vector input which lists the dots for a complete surround, find dots with the same y-axis.
            this becomes the MIN & MAX values for x for the said row, in which we check all existing dots in between.
            increment counter if captured a dot.
            repeat from MIN y to MAX y
    */
    int captureDot(vector<Dot> surroundingDots, int capturedPlayer);

    int searchDot(vector<Dot> dots, int x, int y);

    void eatDot(vector<Dot> surroundDots, int x, int y);
    vector<Dot> m_dots;
    vector<Dot> m_chain;
    int m_currentPlayer;
    Stage m_stage;
};

#endif // ENGINE_H
