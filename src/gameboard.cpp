#include "gameboard.h"
#include <QPainter>
#include <algorithm>
#include <limits>
#include "gameengine.h"
#include "stroke.h"
#include "line.h"

GameBoard::GameBoard(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_engine(0)
    , m_gridStroke(new Stroke())
    , m_gridRotation(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    connect(this, SIGNAL(widthChanged()), SLOT(resizeBoard()));
    connect(this, SIGNAL(heightChanged()), SLOT(resizeBoard()));
    connect(m_gridStroke, SIGNAL(colorChanged()), SLOT(drawBoard()));
    connect(m_gridStroke, SIGNAL(widthChanged()), SLOT(drawBoard()));
    connect(this, SIGNAL(hasPendingMovesChanged()), SLOT(drawBoard()));
}

GameBoard::~GameBoard()
{
    delete m_gridStroke;

    for (int i = 0; i < NUM_PLAYERS; ++i) {
         delete m_dotSvgRenderers[i];
    }
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
        connect(m_engine, SIGNAL(chainsChanged()), SLOT(drawBoard()));
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

    if (listSize == NUM_PLAYERS) {
        for (int i = 0; i < NUM_PLAYERS; ++i) {
            delete m_dotSvgRenderers[i];

            QString fileString = list[i].toString();
            fileString.replace("qrc", "");
            m_dotSvgRenderers[i] = new QSvgRenderer(fileString);
        }
    }
}

QDeclarativeListProperty<Stroke> GameBoard::markStrokes()
{
    return QDeclarativeListProperty<Stroke>(this, m_markStrokes);
}

Stroke *GameBoard::gridStroke() const
{
    return m_gridStroke;
}

bool GameBoard::hasPendingMoves() const
{
    if (m_engine == 0) {
        return false;
    }

    switch (m_engine->stage()) {
    case GameEngine::PlaceDotStage:
        return m_provisionalDot.isValid();
    case GameEngine::ConnectDotsStage:
        return (m_provisionalChain.size() > 1);
    default:
        return false;
    }
}

void GameBoard::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (m_engine == 0) {
        return;
    }

    bool oldAA = painter->testRenderHint(QPainter::Antialiasing);
    bool oldSmooth = painter->testRenderHint(QPainter::SmoothPixmapTransform);

    if(smooth()) {
        painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, true);
    }

    if (!m_gridLines.isEmpty()) {
        painter->setPen(QPen(m_gridStroke->color(), m_gridStroke->width()));
        painter->drawLines(m_gridLines.data(), m_gridLines.size());

        // reset the painter's pen
        painter->setPen(QPen());
    }

    {
        const std::deque<Dot *> &dots = m_engine->getDots();
        std::deque<Dot *>::const_iterator it;
        std::deque<Dot *>::const_iterator end = dots.end();

        for (it = dots.begin(); it != end; ++it) {
            const Dot &dot = **it;
            const QImage &dotImage = m_dotImages[dot.player()];
            QPointF intersection = findIntersection(dot.x(), dot.y());
            intersection = QTransform().rotate(m_gridRotation).map(intersection);
            intersection -= QPointF(dotImage.width() * 0.5, dotImage.height() * 0.5);

            painter->drawImage(intersection, dotImage);
        }
    }

    {
        QVector<QLineF> displayLines;
        std::vector<const Line *>::const_iterator it;
        std::vector<const Line *>::const_iterator end;
        QPointF points[2];
        Stroke *stroke;

        for (int player = 0; player < NUM_PLAYERS; ++player) {
            displayLines.clear();
            const std::vector<const Line *> &lines = m_engine->getLines(player);
            end = lines.end();

            for (it = lines.begin(); it != end; ++it) {
                const Line &line = **it;
                const Dot dots[2] = { line.endpoint1(), line.endpoint2() };

                for (int i = 0; i < 2; ++i) {
                    points[i] = findIntersection(dots[i].x(), dots[i].y());
                    points[i] = QTransform().rotate(m_gridRotation).map(points[i]);
                }
                displayLines.append(QLineF(points[0], points[1]));
            }

            stroke = m_markStrokes[player];
            painter->setPen(QPen(stroke->color(), stroke->width()));
            painter->drawLines(displayLines);
        }

        // reset the painter's pen
        painter->setPen(QPen());
    }

    {
        QVector<QLineF> displayChains;
        const std::vector<const std::deque<Dot *>*> &chains = m_engine->getChains();
        std::vector<const std::deque<Dot *>*>::const_iterator chains_it;
        std::vector<const std::deque<Dot *>*>::const_iterator chains_end = chains.end();
        const std::deque<Dot *> *chain;
        std::deque<Dot *>::const_iterator it;
        std::deque<Dot *>::const_iterator end;
        Dot dots[2];
        QPointF points[2];
        Stroke *stroke;

        for (chains_it = chains.begin(); chains_it != chains_end; ++chains_it) {
            chain = *chains_it;
            end = chain->end();

            for (it = chain->begin(); it != end - 1; ++it) {
                for (int i = 0; i < 2; ++i) {
                    dots[i] = **(it + i);

                    points[i] = findIntersection(dots[i].x(), dots[i].y());
                    points[i] = QTransform().rotate(m_gridRotation).map(points[i]);
                }

                displayChains.append(QLineF(points[0], points[1]));
            }
        }

        stroke = m_markStrokes[m_engine->currentPlayer()];
        painter->setPen(QPen(stroke->color(), stroke->width(), Qt::DotLine));
        painter->drawLines(displayChains);

        // reset the painter's pen
        painter->setPen(QPen());
    }

    if (m_provisionalDot.isValid()) {
        const QImage &dotImage = m_dotImages[m_engine->currentPlayer()];
        QPointF intersection = findIntersection(m_provisionalDot.x(), m_provisionalDot.y());
        QTransform gridDisplayTransform;
        gridDisplayTransform.rotate(m_gridRotation);
        intersection = gridDisplayTransform.map(intersection);
        intersection -= QPointF(dotImage.width() * 0.5, dotImage.height() * 0.5);

        painter->setOpacity(0.5);
        painter->drawImage(intersection, dotImage);
    }

    if (smooth()) {
        painter->setRenderHint(QPainter::Antialiasing, oldAA);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, oldSmooth);
    }
}

void GameBoard::markPosition(QPoint point)
{
    if (m_engine == 0) {
        return;
    }

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

    Dot dot;
    QPointF intersections[2][2];
    QPointF *nearestIntersection = 0;
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

            if (nearestIntersection == 0 || distance < shortestDistance) {
                dot = Dot(m_engine->currentPlayer(), col + j, row + i);
                nearestIntersection = &(intersections[i][j]);
                shortestDistance = distance;
            }
        }
    }

    switch (m_engine->stage()) {
    case GameEngine::PlaceDotStage:
        if (m_engine->canPlaceDot(dot.x(), dot.y())) {
            m_provisionalDot = dot;
            emit hasPendingMovesChanged();
        }
        break;
    case GameEngine::ConnectDotsStage:
        tryAddToChain(dot);
        break;
    default:
        break;
    }
}

void GameBoard::acceptMove(bool accepted)
{
    if (!hasPendingMoves()) {
        return;
    }

    switch (m_engine->stage()) {
    case GameEngine::PlaceDotStage:
        if (accepted) {
            m_engine->placeDot(m_provisionalDot.x(), m_provisionalDot.y());
        }

        m_provisionalDot = Dot();
        break;
    case GameEngine::ConnectDotsStage:
        if (accepted) {
            QList<Dot>::const_iterator it;
            QList<Dot>::const_iterator last = m_provisionalChain.end() - 1;

            for (it = m_provisionalChain.begin(); it != last; ++it) {
                const Dot dot1 = *it;
                const Dot dot2 = *(it + 1);

                m_engine->connectDots(dot1.x(), dot1.y(), dot2.x(), dot2.y());
            }
        }

        m_provisionalChain.clear();
        break;
    default:
        break;
    }

    emit hasPendingMovesChanged();
}

void GameBoard::setUpBoard()
{
    resizeBoard();
    m_provisionalDot = Dot();
}

void GameBoard::resizeBoard()
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

    makeGrid();
    makeDotImages();
    update();
}

void GameBoard::drawBoard()
{
    update();
}

void GameBoard::makeGrid()
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
}

void GameBoard::makeDotImages()
{
    QPainter dotPainter;

    for (int i = 0; i < NUM_PLAYERS; ++i) {
        QImage &dotImage = m_dotImages[i];
        dotImage = QImage(QSize(m_gridSize * 0.5, m_gridSize * 0.5), QImage::Format_ARGB32_Premultiplied);
        dotImage.fill(qRgba(0, 0, 0, 0));
        dotPainter.begin(&dotImage);
        dotPainter.setRenderHint(QPainter::Antialiasing, true);
        m_dotSvgRenderers[i]->render(&dotPainter);
        dotPainter.end();
    }
}

void GameBoard::tryAddToChain(const Dot &dot)
{
    const Dot *existingDot = m_engine->getDotAt(dot.x(), dot.y());

    // check if the dot exists, is active and belongs to the current player
    if (existingDot == 0 || !existingDot->isActive() || existingDot->player() != dot.player()) {
        return;
    }

    // check if there is already a chain
    if (m_provisionalChain.empty()) {
        m_provisionalChain.push_back(dot);
        return;
    }

    const Dot &first = m_provisionalChain.front();
    const Dot &last = m_provisionalChain.back();
    const Dot *previousDot = 0;

    if (dot.isNeighbor(first)) {
        previousDot = &first;
    }
    else if (dot.isNeighbor(last)) {
        previousDot = &last;
    }
    else {
        m_provisionalChain.clear();
        m_provisionalChain.push_back(dot);
        emit hasPendingMovesChanged();
        return;
    }

    if (m_engine->canConnectDots(dot.x(), dot.y(), previousDot->x(), previousDot->y())) {
        if (previousDot == &first) {
            m_provisionalChain.push_front(dot);
        }
        else if (previousDot == &last) {
            m_provisionalChain.push_back(dot);
        }

        emit hasPendingMovesChanged();
    }
}

QPointF GameBoard::findIntersection(int x, int y) const
{
    QRectF grid = QTransform().rotate(-m_gridRotation).mapRect(m_gridRect);
    QPointF gridOrigin = grid.topLeft();

    return QPointF(x * m_gridSize, y * m_gridSize) + gridOrigin;
}
