#include "gamegrid.h"
#include <QLineF>
#include <QPainter>
#include <QRectF>
#include <QVarLengthArray>
#include "stroke.h"

GameGrid::GameGrid(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_rows(1)
    , m_columns(1)
    , m_stroke(new Stroke())
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    connect(m_stroke, SIGNAL(colorChanged()), SLOT(drawGrid()));
    connect(m_stroke, SIGNAL(widthChanged()), SLOT(drawGrid()));
}

GameGrid::~GameGrid()
{
    delete m_stroke;
}

int GameGrid::rows() const
{
    return m_rows;
}

void GameGrid::setRows(int rows)
{
    if (rows != m_rows) {
        m_rows = rows;
        update();
    }
}

int GameGrid::columns() const
{
    return m_columns;
}

void GameGrid::setColumns(int columns)
{
    if (columns != m_columns) {
        m_columns = columns;
        update();
    }
}

Stroke *GameGrid::stroke() const
{
    return m_stroke;
}

void GameGrid::drawGrid()
{
    update();
}

void GameGrid::drawChain()
{
    // draw the current chain
}

void GameGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF rect = boundingRect();
    qreal gridSize = qMin(rect.width() / (m_columns + 2), rect.height() / (m_rows + 2));
    qreal width = m_columns * gridSize;
    qreal height = m_rows * gridSize;
    qreal left = rect.left() + (rect.width() - width) / 2;
    qreal right = left + width + 1;
    qreal top = rect.top() + (rect.height() - height) / 2;
    qreal bottom = top + height + 1;

    QVarLengthArray<QLineF, 100> lines;

    for (qreal x = left; x <= right; x += gridSize)
        lines.append(QLineF(x, top, x, bottom));
    for (qreal y = top; y <= bottom; y += gridSize)
        lines.append(QLineF(left, y, right, y));

    bool oldAA = painter->testRenderHint(QPainter::Antialiasing);
    bool oldSmooth = painter->testRenderHint(QPainter::SmoothPixmapTransform);

    if(smooth())
        painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, true);

    QPen pen(m_stroke->color(), m_stroke->width());

    painter->setPen(pen);
    painter->drawLines(lines.data(), lines.size());

    if (smooth()) {
        painter->setRenderHint(QPainter::Antialiasing, oldAA);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, oldSmooth);
    }
}
