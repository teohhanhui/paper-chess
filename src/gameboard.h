#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "dot.h"
#include <QImage>
#include <QList>
#include <QQuickItem>
#include <QSGGeometryNode>
#include <QSGMaterial>
#include <QSGNode>
#include <QSGOpacityNode>
#include <QSGTexture>
#include <QSvgRenderer>
#include <QVarLengthArray>
#include <QVector>
#include <deque>

class Stroke;
class GameEngine;

class GameBoard : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(GameEngine *engine READ engine WRITE setEngine)
    Q_PROPERTY(QVariantList dotSources READ dotSources WRITE setDotSources)
    Q_PROPERTY(QQmlListProperty<Stroke> markStrokes READ markStrokes)
    Q_PROPERTY(Stroke *gridStroke READ gridStroke)
    Q_PROPERTY(
        bool hasPendingMoves READ hasPendingMoves NOTIFY hasPendingMovesChanged)

public:
    explicit GameBoard(QQuickItem *parent = nullptr);
    ~GameBoard() override;

    GameEngine *engine() const;
    void setEngine(GameEngine *engine);

    QVariantList dotSources() const;
    void setDotSources(QVariantList &list);

    QQmlListProperty<Stroke> markStrokes();
    void appendMarkStroke(Stroke *markStroke);
    int markStrokeCount() const;
    Stroke *markStroke(int index) const;
    void clearMarkStrokes();

    Stroke *gridStroke() const;

    bool hasPendingMoves() const;

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

protected:
    void componentComplete() override;
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;

private:
    class QSGGameBoardNode : public QSGNode
    {
    public:
        explicit QSGGameBoardNode();
        ~QSGGameBoardNode();

        QSGGeometryNode *gridNode() const;
        QSGNode *dotContainerNode() const;
        QSGNode *lineContainerNode() const;
        QSGNode *chainContainerNode() const;
        QSGOpacityNode *provisionalDotContainerNode() const;
        QSGOpacityNode *provisionalChainContainerNode() const;

        QVector<QSGTexture *> dotTextures() const;
        void setDotTextures(QVector<QSGTexture *> dotTextures);

        QVector<QSGMaterial *> lineMaterials() const;
        void setLineMaterials(QVector<QSGMaterial *> lineMaterials);

    private:
        QSGGeometryNode *m_gridNode;
        QSGNode *m_dotContainerNode;
        QSGNode *m_lineContainerNode;
        QSGNode *m_chainContainerNode;
        QSGOpacityNode *m_provisionalDotContainerNode;
        QSGOpacityNode *m_provisionalChainContainerNode;
        QVector<QSGTexture *> m_dotTextures;
        QVector<QSGMaterial *> m_lineMaterials;
    };

    static void appendMarkStroke(
        QQmlListProperty<Stroke> *property,
        Stroke *value);
    static int markStrokeCount(QQmlListProperty<Stroke> *property);
    static Stroke *markStroke(QQmlListProperty<Stroke> *property, int index);
    static void clearMarkStrokes(QQmlListProperty<Stroke> *property);

    void makeGrid();
    void makeDotImages();
    void tryAddToChain(const Dot &dot);
    QPointF findIntersection(int x, int y) const;

    bool isReady() const;

    void updateGridNode(QSGGameBoardNode *node);
    void updateDotContainerNode(QSGGameBoardNode *node);
    void updateLineContainerNode(QSGGameBoardNode *node);
    void updateChainContainerNode(QSGGameBoardNode *node);
    void updateProvisionalDotContainerNode(QSGGameBoardNode *node);
    void updateProvisionalChainContainerNode(QSGGameBoardNode *node);

    void prepareDotTextures(QSGGameBoardNode *node);
    void prepareLineMaterials(QSGGameBoardNode *node);

    QTransform gridDisplayTransform() const;

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
    bool m_gridDirty;
    bool m_dotsDirty;
    bool m_dotImagesDirty;
    bool m_linesDirty;
    bool m_lineMaterialsDirty;
};

#endif // GAMEBOARD_H
