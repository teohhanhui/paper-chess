#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QQuickPaintedItem>
#include <QVarLengthArray>
#include <QSvgRenderer>
#include <QImage>
#include <QList>
#include <deque>
#include "dot.h"

class Stroke;
class GameEngine;

class GameBoard : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(GameEngine *engine READ engine WRITE setEngine)
    Q_PROPERTY(QVariantList dotSources READ dotSources WRITE setDotSources)
    Q_PROPERTY(QQmlListProperty<Stroke> markStrokes READ markStrokes)
    Q_PROPERTY(Stroke *gridStroke READ gridStroke)
    Q_PROPERTY(bool hasPendingMoves READ hasPendingMoves NOTIFY hasPendingMovesChanged)

public:
    explicit GameBoard(QQuickPaintedItem *parent = nullptr);
    ~GameBoard();

    GameEngine *engine() const;
    void setEngine(GameEngine *engine);

    QVariantList dotSources() const;
    void setDotSources(QVariantList &list);

    QQmlListProperty<Stroke> markStrokes();

    Stroke *gridStroke() const;

    bool hasPendingMoves() const;

    void paint(QPainter *painter);

public slots:
    void markPosition(QPoint pos);
    void acceptMove(bool accepted = true);

signals:
    void hasPendingMovesChanged();

protected slots:
    void setUpBoard();
    void resizeBoard();
    void drawBoard();
    void clearProvisional();

private:
    void makeGrid();
    void makeDotImages();
    void tryAddToChain(const Dot &dot);
    QPointF findIntersection(int x, int y) const;

    static const int DEFAULT_NUM_PLAYERS = 2;

    GameEngine *m_engine;
    int m_numPlayers;
    QVariantList m_dotSources;
    QList<Stroke *> m_markStrokes;
    Stroke *m_gridStroke;
    int m_gridRows;
    int m_gridColumns;
    qreal m_gridRotation;
    qreal m_gridSize;
    QRectF m_gridRect;
    QVarLengthArray<QLineF, 100> m_gridLines;
    QVarLengthArray<QSvgRenderer *, DEFAULT_NUM_PLAYERS> m_dotSvgRenderers;
    QVarLengthArray<QImage, DEFAULT_NUM_PLAYERS> m_dotImages;
    Dot m_provisionalDot;
    std::deque<Dot> m_provisionalChain;
};

#endif // GAMEBOARD_H
