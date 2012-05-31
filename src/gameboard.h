#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QDeclarativeItem>
#include <QVarLengthArray>
#include <QPointF>
#include <QPoint>

class Stroke;
class GameEngine;

class GameBoard : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(GameEngine *engine READ engine WRITE setEngine)
    Q_PROPERTY(Stroke *gridStroke READ gridStroke)

public:
    explicit GameBoard(QDeclarativeItem *parent = 0);
    ~GameBoard();

    GameEngine *engine() const;
    void setEngine(GameEngine *engine);

    Stroke *gridStroke() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

signals:
    
public slots:
    void updateGrid();
    void drawGrid();
    void markPosition(QPoint pos);

protected slots:
    void setUpBoard();
    void resizeGrid();

private:
    static const int SHAPE_CORNERS = 4;

    GameEngine *m_engine;
    Stroke *m_gridStroke;
    int m_gridRows;
    int m_gridColumns;
    qreal m_gridRotation;
    qreal m_gridSize;
    QRectF m_gridRect;
    QVarLengthArray<QLineF, 100> m_gridLines;
    QPointF m_markedPoint;
    QPoint m_provisionalDot;
};

#endif // GAMEBOARD_H
