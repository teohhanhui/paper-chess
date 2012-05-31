#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QDeclarativeItem>
#include <QVarLengthArray>
#include <QPoint>
#include <QtSvg/QSvgRenderer>

class Stroke;
class GameEngine;

class GameBoard : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(GameEngine *engine READ engine WRITE setEngine)
    Q_PROPERTY(QVariantList dotSources READ dotSources WRITE setDotSources)
    Q_PROPERTY(Stroke *gridStroke READ gridStroke)

public:
    explicit GameBoard(QDeclarativeItem *parent = 0);
    ~GameBoard();

    GameEngine *engine() const;
    void setEngine(GameEngine *engine);

    QVariantList dotSources() const;
    void setDotSources(QVariantList &list);

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
    void beginTurn();

private:
    QPointF findIntersection(int x, int y) const;

    const QPoint m_invalidDot;

    GameEngine *m_engine;
    QVariantList m_dotSources;
    Stroke *m_gridStroke;
    int m_gridRows;
    int m_gridColumns;
    qreal m_gridRotation;
    qreal m_gridSize;
    QRectF m_gridRect;
    QVarLengthArray<QLineF, 100> m_gridLines;
    QPoint m_provisionalDot;
    QSvgRenderer *m_dotSvgRenderers[2];
};

#endif // GAMEBOARD_H
