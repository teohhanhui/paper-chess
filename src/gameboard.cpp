#include "gameboard.h"
#include <QPainter>
#include <algorithm>
#include <limits>
#include "gameengine.h"
#include "stroke.h"

GameBoard::GameBoard(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_invalidDot(QPoint(-1, -1))
    , m_engine(0)
    , m_gridStroke(new Stroke())
    , m_gridRotation(0)
    , m_provisionalDot(m_invalidDot)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    connect(this, SIGNAL(widthChanged()), SLOT(resizeGrid()));
    connect(this, SIGNAL(heightChanged()), SLOT(resizeGrid()));
    connect(m_gridStroke, SIGNAL(colorChanged()), SLOT(drawGrid()));
    connect(m_gridStroke, SIGNAL(widthChanged()), SLOT(drawGrid()));
}

GameBoard::~GameBoard()
{
    delete m_gridStroke;
}

GameEngine *GameBoard::engine() const
{
    return m_engine;
}

void GameBoard::setEngine(GameEngine *engine)
{
    if (engine == m_engine) {
        return;
    }

    if (m_engine != 0) {
        m_engine->disconnect(this);
    }

    if (engine != 0) {
        m_engine = engine;
        connect(m_engine, SIGNAL(gameStarted()), SLOT(setUpBoard()));
        connect(m_engine, SIGNAL(turnsLeftChanged()), SLOT(beginTurn()));
        setUpBoard();
    }
}

QVariantList GameBoard::dotSources() const
{
    return m_dotSources;
}

void GameBoard::setDotSources(QVariantList &list)
{
    m_dotSources = list;

    int listSize = list.size();

    if (listSize == 2) {
        for (int i = 0; i < listSize; ++i) {
            delete m_dotSvgRenderers[i];

            QString fileString = list[i].toString();
            fileString.replace("qrc", "");
            m_dotSvgRenderers[i] = new QSvgRenderer(fileString);
        }
    }
}

Stroke *GameBoard::gridStroke() const
{
    return m_gridStroke;
}

void GameBoard::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    bool oldAA = painter->testRenderHint(QPainter::Antialiasing);
    bool oldSmooth = painter->testRenderHint(QPainter::SmoothPixmapTransform);

    if(smooth()) {
        painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, true);
    }

    painter->setPen(QPen(m_gridStroke->color(), m_gridStroke->width()));
    painter->drawLines(m_gridLines.data(), m_gridLines.size());

    if (m_provisionalDot != m_invalidDot) {
        QImage dotImage(QSize(m_gridSize * 0.5, m_gridSize * 0.5), QImage::Format_ARGB32_Premultiplied);
        dotImage.fill(qRgba(0, 0, 0, 0));
        QPainter dotPainter;
        dotPainter.begin(&dotImage);
        dotPainter.setRenderHint(QPainter::Antialiasing, true);
        m_dotSvgRenderers[m_engine->currentPlayer()]->render(&dotPainter);
        dotPainter.end();

        QPointF intersection = findIntersection(m_provisionalDot.x(), m_provisionalDot.y());
        QTransform gridDisplayTransform;
        gridDisplayTransform.rotate(m_gridRotation);
        intersection = gridDisplayTransform.map(intersection);
        intersection -= QPointF(dotImage.width() / 2, dotImage.height() / 2);

        painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, true);
        painter->drawImage(intersection, dotImage);
    }

    if (smooth()) {
        painter->setRenderHint(QPainter::Antialiasing, oldAA);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, oldSmooth);
    }
}

void GameBoard::updateGrid()
{
    QRectF rect = boundingRect();

    if (rect.isNull()) {
        return;
    }

    m_gridSize = qMin(rect.width() / (m_gridColumns + 2), rect.height() / (m_gridRows + 2));

    qreal width = m_gridColumns * m_gridSize;
    qreal height = m_gridRows * m_gridSize;
    qreal left = rect.left() + (rect.width() - width) / 2;
    qreal right = left + width + 1;
    qreal top = rect.top() + (rect.height() - height) / 2;
    qreal bottom = top + height + 1;

    m_gridRect = QRectF(left, top, width, height);

    m_gridLines.clear();

    for (qreal x = left; x <= right; x += m_gridSize) {
        m_gridLines.append(QLineF(x, top, x, bottom));
    }
    for (qreal y = top; y <= bottom; y += m_gridSize) {
        m_gridLines.append(QLineF(left, y, right, y));
    }

    update();
}

void GameBoard::drawGrid()
{
    update();
}

void GameBoard::markPosition(QPoint point)
{
    // check if the point is around the grid
    if (!(m_gridRect.adjusted(-m_gridSize, -m_gridSize, m_gridSize, m_gridSize)).contains(point)) {
        return;
    }

    QTransform gridTransform;
    gridTransform.rotate(-m_gridRotation);

    QRectF grid = gridTransform.mapRect(m_gridRect);
    point = gridTransform.map(point);

    QPointF gridOrigin = grid.topLeft();
    int row = (point.y() - gridOrigin.y()) / m_gridSize;
    int col = (point.x() - gridOrigin.x()) / m_gridSize;

    QPointF intersections[2][2];
    QPoint nearestIntersection;
    qreal shortestDistance = std::numeric_limits<qreal>::max();
    qreal distance;

    intersections[0][0] = findIntersection(col, row);
    intersections[0][1] = findIntersection(col + 1, row);
    intersections[1][0] = findIntersection(col, row + 1);
    intersections[1][1] = findIntersection(col + 1, row + 1);

    // check the distance of the point from each intersection
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            distance = (intersections[i][j] - point).manhattanLength();

            if ((i == 0 && j == 0) || distance < shortestDistance) {
                nearestIntersection = QPoint(col + j, row + i);
                shortestDistance = distance;
            }
        }
    }
    qDebug() << "Dot: (" << nearestIntersection.x() << "," << nearestIntersection.y() << ")";
    switch (m_engine->stage()) {
    case GameEngine::PlaceDotStage:
        // check if this is the provisional dot
        // place the dot if that is the case
        if (nearestIntersection == m_provisionalDot) {
            m_engine->placeDot(col, row);
        }
        else {
            m_provisionalDot = nearestIntersection;
            update();
        }
        break;
    case GameEngine::ConnectingStage:
        break;
    }
}

void GameBoard::setUpBoard()
{
    resizeGrid();
    m_provisionalDot = m_invalidDot;
}

void GameBoard::resizeGrid()
{
    if (m_engine == 0 || width() == 0 || height() == 0) {
        return;
    }

    int rows = m_engine->rows();
    int cols = m_engine->columns();
    int shorter = std::min(rows, cols);
    int taller = std::max(rows, cols);

    if (width() < height()) {
        m_gridRows = taller;
        m_gridColumns = shorter;

        m_gridRotation = rows == taller ? 0 : 90;
    }
    else {
        m_gridRows = shorter;
        m_gridColumns = taller;

        m_gridRotation = rows == shorter ? 0 : -90;
    }

    updateGrid();
}

void GameBoard::beginTurn()
{
    m_provisionalDot = m_invalidDot;
}

QPointF GameBoard::findIntersection(int x, int y) const
{
    QRectF grid = QTransform().rotate(-m_gridRotation).mapRect(m_gridRect);
    QPointF gridOrigin = grid.topLeft();

    return QPointF(x * m_gridSize, y * m_gridSize) + gridOrigin;
}
