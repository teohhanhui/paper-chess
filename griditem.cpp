#include "griditem.h"
#include <QLineF>
#include <QPainter>
#include <QRectF>
#include <QVarLengthArray>
#include "stroke.h"

GridItem::GridItem(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_rows(1)
    , m_columns(1)
    , m_stroke(new Stroke())
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    connect(m_stroke, SIGNAL(colorChanged()), SLOT(strokeLines()));
    connect(m_stroke, SIGNAL(widthChanged()), SLOT(strokeLines()));
}

GridItem::~GridItem()
{
    delete m_stroke;
}

int GridItem::rows() const
{
    return m_rows;
}

void GridItem::setRows(const int rows)
{
    if (rows != m_rows) {
        m_rows = rows;
        update();
    }
}

int GridItem::columns() const
{
    return m_columns;
}

void GridItem::setColumns(const int columns)
{
    if (columns != m_columns) {
        m_columns = columns;
        update();
    }
}

Stroke *GridItem::stroke() const
{
    return m_stroke;
}

void GridItem::strokeLines()
{
    update();
}

void GridItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QRectF rect = boundingRect();
    qreal gridSize = qMin(rect.width() / m_columns, rect.height() / m_rows);
    qreal width = m_columns * gridSize;
    qreal height = m_rows * gridSize;
    qreal left = rect.left() + (rect.width() - width) / 2;
    qreal right = left + width;
    qreal top = rect.top() + (rect.height() - height) / 2;
    qreal bottom = top + height;

    QVarLengthArray<QLineF, 100> lines;

    for (qreal x = left; x <= right; x += gridSize)
        lines.append(QLineF(x, top, x, bottom));
    for (qreal y = top; y <= bottom; y += gridSize)
        lines.append(QLineF(left, y, right, y));

    /*
    qDebug() << "Width: " << width;
    qDebug() << "Height: " << height;
    qDebug() << "Left: " << left;
    qDebug() << "Right: " << right;
    qDebug() << "Top: " << top;
    qDebug() << "Bottom: " << bottom;
    qDebug() << "No. of lines: " << lines.size();
    */

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
