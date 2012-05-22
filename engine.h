#ifndef ENGINE_H
#define ENGINE_H
#include "dot.h"
#include <QObject>
#include <vector>
class Engine : public QObject{
public:
    explicit Engine(QObject *parent = 0);


    /*iterates through surround chain & check dot status & dot player
    returns 1 for surround if able, 0 if fail*/
    Q_INVOKABLE int surroundCheck(vector<Dot> chain, int player);

    /*check number of captured dots, returns number of dots captured
    idea of calculating captured dots: with limits of MAX x, MAX y, MIN x, MIN y
    we can visualize the captured dots in a shape.  based on the coordinates,
    the captured dots will be calculated line by line.
    Steps: based on vector input which lists the dots for a complete surround, find dots with the same y-axis.
            this becomes the MIN & MAX values for x for the said row, in which we check all existing dots in between.
            increment counter if captured a dot.
            repeat from MIN y to MAX y
    */
    Q_INVOKABLE int captureDot(vector<Dot> surroundingDots);

    Dot searchDot(vector<Dot> playerDots, Dot target);

private:
    vector<Dot> player1Dots;
    vector<Dot> player2Dots;
};

#endif // ENGINE_H
