#include "gameboard.h"
#include <QPainter>
#include <QSGFlatColorMaterial>
#include <QQuickWindow>
#include <QSGImageNode>
#include <algorithm>
#include <limits>
#include "gameengine.h"
#include "stroke.h"
#include "line.h"

GameBoard::QSGGameBoardNode::QSGGameBoardNode()
    : QSGNode()
    , m_gridNode(new QSGGeometryNode())
    , m_dotContainerNode(new QSGNode())
    , m_lineContainerNode(new QSGNode())
    , m_chainContainerNode(new QSGNode())
    , m_provisionalDotContainerNode(new QSGOpacityNode())
    , m_provisionalChainContainerNode(new QSGOpacityNode())
    , m_dotTextures(QVector<QSGTexture *>(DEFAULT_NUM_PLAYERS))
    , m_lineMaterials(QVector<QSGMaterial *>(DEFAULT_NUM_PLAYERS))
{
    appendChildNode(m_gridNode);
    appendChildNode(m_dotContainerNode);
    appendChildNode(m_lineContainerNode);
    appendChildNode(m_chainContainerNode);
    appendChildNode(m_provisionalDotContainerNode);
    appendChildNode(m_provisionalChainContainerNode);
}

GameBoard::QSGGameBoardNode::~QSGGameBoardNode()
{
    for (const QSGTexture *dotTexture : m_dotTextures) {
        delete dotTexture;
    }

    for (const QSGMaterial *lineMaterial : m_lineMaterials) {
        delete lineMaterial;
    }
}

QSGGeometryNode *GameBoard::QSGGameBoardNode::gridNode() const
{
    return m_gridNode;
}

QSGNode *GameBoard::QSGGameBoardNode::dotContainerNode() const
{
    return m_dotContainerNode;
}

QSGNode *GameBoard::QSGGameBoardNode::lineContainerNode() const
{
    return m_lineContainerNode;
}

QSGNode *GameBoard::QSGGameBoardNode::chainContainerNode() const
{
    return m_chainContainerNode;
}

QSGOpacityNode *GameBoard::QSGGameBoardNode::provisionalDotContainerNode() const
{
    return m_provisionalDotContainerNode;
}

QSGOpacityNode *GameBoard::QSGGameBoardNode::provisionalChainContainerNode() const
{
    return m_provisionalChainContainerNode;
}

QVector<QSGTexture *> GameBoard::QSGGameBoardNode::dotTextures() const
{
    return m_dotTextures;
}

void GameBoard::QSGGameBoardNode::setDotTextures(QVector<QSGTexture *> dotTextures)
{
    m_dotTextures = dotTextures;
}

QVector<QSGMaterial *> GameBoard::QSGGameBoardNode::lineMaterials() const
{
    return m_lineMaterials;
}

void GameBoard::QSGGameBoardNode::setLineMaterials(QVector<QSGMaterial *> lineMaterials)
{
    m_lineMaterials = lineMaterials;
}

GameBoard::GameBoard(QQuickItem *parent)
    : QQuickItem(parent)
    , m_engine(nullptr)
    , m_numPlayers(DEFAULT_NUM_PLAYERS)
    , m_gridStroke(new Stroke())
    , m_gridRotation(0)
    , m_gridDirty(true)
    , m_dotsDirty(true)
    , m_dotImagesDirty(true)
    , m_linesDirty(true)
    , m_lineMaterialsDirty(true)
{
    setFlag(ItemHasContents, true);
    connect(this, &GameBoard::widthChanged, this, &GameBoard::resizeBoard);
    connect(this, &GameBoard::heightChanged, this, &GameBoard::resizeBoard);
    connect(this, &GameBoard::hasPendingMovesChanged, this, &GameBoard::drawBoard);
}

GameBoard::~GameBoard()
{
    delete m_gridStroke;

    for (const QSvgRenderer *dotSvgRenderer : m_dotSvgRenderers) {
        delete dotSvgRenderer;
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

    if (m_engine != nullptr) {
        m_engine->disconnect(this);
    }

    if (engine != nullptr) {
        m_engine = engine;
        m_numPlayers = engine->numPlayers();

        connect(m_engine, &GameEngine::gameStarted, this, &GameBoard::setUpBoard);
        connect(m_engine, &GameEngine::chainsChanged, this, &GameBoard::drawBoard);
        connect(m_engine, &GameEngine::turnEnded, this, &GameBoard::clearProvisional);

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

    if (list.size() == m_numPlayers) {
        for (int i = 0; i < m_dotSvgRenderers.size(); ++i) {
            delete m_dotSvgRenderers[i];
        }

        m_dotSvgRenderers.resize(m_numPlayers);

        for (int i = 0; i < m_numPlayers; ++i) {
            QString fileString = list[i].toString();
            fileString.replace("qrc", "");
            m_dotSvgRenderers[i] = new QSvgRenderer(fileString);
        }
    }
}

QQmlListProperty<Stroke> GameBoard::markStrokes()
{
    return QQmlListProperty<Stroke>(this, m_markStrokes);
}

Stroke *GameBoard::gridStroke() const
{
    return m_gridStroke;
}

bool GameBoard::hasPendingMoves() const
{
    if (!isReady()) {
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

void GameBoard::markPosition(QPoint point)
{
    if (!isReady()) {
        return;
    }

    // check if the point is around the grid
    if (!(m_gridRect.adjusted(-m_gridSize, -m_gridSize, m_gridSize, m_gridSize)).contains(point)) {
        return;
    }

    const QTransform gridTransform = QTransform().rotate(-m_gridRotation);

    const QRectF grid = gridTransform.mapRect(m_gridRect);
    point = gridTransform.map(point);

    const QPointF gridOrigin = grid.topLeft();
    int row = static_cast<int>((point.y() - gridOrigin.y()) / m_gridSize);
    int col = static_cast<int>((point.x() - gridOrigin.x()) / m_gridSize);

    Dot dot;
    QPointF intersections[2][2];
    QPointF *nearestIntersection = nullptr;
    qreal shortestDistance = std::numeric_limits<qreal>::max();

    intersections[0][0] = findIntersection(col, row);
    intersections[0][1] = findIntersection(col + 1, row);
    intersections[1][0] = findIntersection(col, row + 1);
    intersections[1][1] = findIntersection(col + 1, row + 1);

    // check the distance of the point from each intersection
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            const qreal distance = (intersections[i][j] - point).manhattanLength();

            if (nearestIntersection == nullptr || distance < shortestDistance) {
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
    if (!isReady()) {
        return;
    }

    if (!hasPendingMoves()) {
        return;
    }

    switch (m_engine->stage()) {
    case GameEngine::PlaceDotStage:
        if (accepted) {
            m_engine->placeDot(m_provisionalDot.x(), m_provisionalDot.y());

            m_dotsDirty = true;
        }

        m_provisionalDot = Dot();
        break;
    case GameEngine::ConnectDotsStage:
        if (accepted) {
            std::deque<Dot>::const_iterator it;
            std::deque<Dot>::const_iterator last = m_provisionalChain.end() - 1;

            for (it = m_provisionalChain.begin(); it != last; ++it) {
                const Dot dot1 = *it;
                const Dot dot2 = *(it + 1);

                m_engine->connectDots(dot1.x(), dot1.y(), dot2.x(), dot2.y());
            }

            m_linesDirty = true;
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
    if (!isReady()) {
        return;
    }

    m_lineMaterialsDirty = true;

    resizeBoard();
    clearProvisional();
}

void GameBoard::resizeBoard()
{
    if (!isReady()) {
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

    for (Stroke *stroke : m_markStrokes) {
        stroke->setWidth(m_gridSize * 0.25);
    }

    m_gridDirty = true;
    m_dotsDirty = true;
    m_dotImagesDirty = true;
    m_linesDirty = true;

    update();
}

void GameBoard::drawBoard()
{
    update();
}

void GameBoard::clearProvisional()
{
    m_provisionalDot = Dot();
    m_provisionalChain.clear();

    update();
}

void GameBoard::componentComplete()
{
    QQuickItem::componentComplete();

    setUpBoard();
}

QSGNode *GameBoard::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    if (!isReady()) {
        return oldNode;
    }

    QSGGameBoardNode *node = static_cast<QSGGameBoardNode *>(oldNode);
    if (node == nullptr) {
        node = new QSGGameBoardNode();
    }

    prepareDotTextures(node);
    prepareLineMaterials(node);

    updateGridNode(node);
    updateDotContainerNode(node);
    updateLineContainerNode(node);
    updateChainContainerNode(node);
    updateProvisionalDotContainerNode(node);
    updateProvisionalChainContainerNode(node);

    return node;
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

    m_dotImages.resize(m_numPlayers);

    for (int i = 0; i < m_numPlayers; ++i) {
        QImage &dotImage = m_dotImages[i];
        dotImage = QImage(QSize(static_cast<int>(m_gridSize * 0.5), static_cast<int>(m_gridSize * 0.5)), QImage::Format_ARGB32_Premultiplied);
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
    if (existingDot == nullptr || !existingDot->isActive() || existingDot->player() != dot.player()) {
        return;
    }

    // check if there is already a chain
    if (m_provisionalChain.empty()) {
        m_provisionalChain.push_back(dot);
        return;
    }

    const Dot &first = m_provisionalChain.front();
    const Dot &last = m_provisionalChain.back();
    const Dot *previousDot = nullptr;

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

    std::deque<Dot *> chain;
    std::deque<Dot>::iterator it;
    std::deque<Dot>::iterator end = m_provisionalChain.end();

    for (it = m_provisionalChain.begin(); it != end; ++it) {
        chain.push_back(&*it);
    }

    if (!m_engine->neighborsInChain(chain.begin(), chain.end() - 1, *previousDot, dot)
            && m_engine->canConnectDots(*previousDot, dot)) {
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

bool GameBoard::isReady() const
{
    return m_engine != nullptr && isComponentComplete() && width() > 0 && height() > 0;
}

void GameBoard::updateGridNode(GameBoard::QSGGameBoardNode *node)
{
    if (!m_gridDirty) {
        return;
    }

    QSGGeometryNode *gridNode = node->gridNode();
    QSGGeometry *gridGeometry = gridNode->geometry();

    if (gridGeometry == nullptr) {
        gridGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), m_gridLines.size() * 2);
        gridGeometry->setLineWidth(static_cast<float>(m_gridStroke->width()));
        gridGeometry->setDrawingMode(QSGGeometry::DrawLines);
        gridNode->setGeometry(gridGeometry);
        gridNode->setFlag(QSGNode::OwnsGeometry);

        QSGFlatColorMaterial *gridMaterial = new QSGFlatColorMaterial();
        gridMaterial->setColor(m_gridStroke->color());
        gridNode->setMaterial(gridMaterial);
        gridNode->setFlag(QSGNode::OwnsMaterial);
    }
    else {
        gridGeometry->allocate(m_gridLines.size() * 2);
    }

    QSGGeometry::Point2D *vertices = gridGeometry->vertexDataAsPoint2D();
    size_t i = 0;

    for (const QLineF &line : m_gridLines) {
        vertices[i++].set(static_cast<float>(line.x1()), static_cast<float>(line.y1()));
        vertices[i++].set(static_cast<float>(line.x2()), static_cast<float>(line.y2()));
    }

    gridNode->markDirty(QSGNode::DirtyGeometry);

    m_gridDirty = false;
}

void GameBoard::updateDotContainerNode(GameBoard::QSGGameBoardNode *node)
{
    if (!m_dotsDirty) {
        return;
    }

    QSGNode *dotContainerNode = node->dotContainerNode();
    dotContainerNode->removeAllChildNodes();

    const std::vector<const Dot *> &dots = m_engine->getDots();
    QVector<QSGTexture *> dotTextures = node->dotTextures();
    const QTransform gridDisplayTransform = GameBoard::gridDisplayTransform();

    for (const Dot *pdot : dots) {
        const Dot &dot = *pdot;
        const QImage &dotImage = m_dotImages[dot.player()];
        const QPointF intersection = gridDisplayTransform.map(findIntersection(dot.x(), dot.y()));
        const QRectF dotRect = QRectF((intersection - QPointF(dotImage.width() * 0.5, dotImage.height() * 0.5)), (intersection + QPointF(dotImage.width() * 0.5, dotImage.height() * 0.5)));

        QSGImageNode *dotNode = window()->createImageNode();
        dotNode->setRect(dotRect);
        QSGTexture *dotTexture = dotTextures[dot.player()];
        dotNode->setTexture(dotTexture);
        dotContainerNode->appendChildNode(dotNode);
    }

    m_dotsDirty = false;
}

void GameBoard::updateLineContainerNode(GameBoard::QSGGameBoardNode *node)
{
    if (!m_linesDirty) {
        return;
    }

    QSGNode *lineContainerNode = node->lineContainerNode();
    lineContainerNode->removeAllChildNodes();

    QVector<QSGMaterial *> lineMaterials = node->lineMaterials();
    const QTransform gridDisplayTransform = GameBoard::gridDisplayTransform();

    for (int player = 0; player < m_numPlayers; ++player) {
        const std::vector<const Line *> &lines = m_engine->getLines(player);
        const Stroke *stroke = m_markStrokes[player];
        QSGMaterial *lineMaterial = lineMaterials[player];

        QSGGeometryNode *linesNode = new QSGGeometryNode();
        QSGGeometry *linesGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), static_cast<int>(lines.size() * 2));
        linesGeometry->setLineWidth(static_cast<float>(stroke->width()));
        linesGeometry->setDrawingMode(QSGGeometry::DrawLines);
        linesNode->setGeometry(linesGeometry);
        linesNode->setFlag(QSGNode::OwnsGeometry);
        linesNode->setMaterial(lineMaterial);
        lineContainerNode->appendChildNode(linesNode);

        QSGGeometry::Point2D *vertices = linesGeometry->vertexDataAsPoint2D();
        size_t i = 0;

        for (const Line *pline : lines) {
            const Line &line = *pline;

            for (const Dot &dot : {line.endpoint1(), line.endpoint2()}) {
                QPointF point = gridDisplayTransform.map(findIntersection(dot.x(), dot.y()));
                vertices[i++].set(static_cast<float>(point.x()), static_cast<float>(point.y()));
            }
        }
    }

    m_linesDirty = false;
}

void GameBoard::updateChainContainerNode(GameBoard::QSGGameBoardNode *node)
{
    QSGNode *chainContainerNode = node->chainContainerNode();
    chainContainerNode->removeAllChildNodes();

    const std::vector<std::vector<const Dot *> > &chains = m_engine->getChains();
    Stroke *stroke = m_markStrokes[m_engine->currentPlayer()];
    QVector<QSGMaterial *> lineMaterials = node->lineMaterials();
    QSGMaterial *chainMaterial = lineMaterials[m_engine->currentPlayer()];
    const QTransform gridDisplayTransform = GameBoard::gridDisplayTransform();

    for (const std::vector<const Dot *> &chain : chains) {
        QSGGeometryNode *chainNode = new QSGGeometryNode();
        QSGGeometry *chainGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), static_cast<int>(chain.size()));
        chainGeometry->setLineWidth(static_cast<float>(stroke->width()));
        chainGeometry->setDrawingMode(QSGGeometry::DrawLineStrip);
        chainNode->setGeometry(chainGeometry);
        chainNode->setFlag(QSGNode::OwnsGeometry);
        // painter->setPen(QPen(stroke->color(), stroke->width(), Qt::DashLine, Qt::FlatCap));
        chainNode->setMaterial(chainMaterial);
        chainContainerNode->appendChildNode(chainNode);

        QSGGeometry::Point2D *vertices = chainGeometry->vertexDataAsPoint2D();
        size_t i = 0;

        for (const Dot *pdot : chain) {
            const Dot dot = *pdot;
            const QPointF point = gridDisplayTransform.map(findIntersection(dot.x(), dot.y()));

            vertices[i++].set(static_cast<float>(point.x()), static_cast<float>(point.y()));
        }
    }
}

void GameBoard::updateProvisionalDotContainerNode(GameBoard::QSGGameBoardNode *node)
{
    QSGOpacityNode *provisionalDotContainerNode = node->provisionalDotContainerNode();
    provisionalDotContainerNode->setOpacity(0.5);
    provisionalDotContainerNode->removeAllChildNodes();

    if (m_provisionalDot.isValid()) {
        const QImage &dotImage = m_dotImages[m_engine->currentPlayer()];
        QVector<QSGTexture *> dotTextures = node->dotTextures();
        const QTransform gridDisplayTransform = GameBoard::gridDisplayTransform();
        const QPointF intersection = gridDisplayTransform.map(findIntersection(m_provisionalDot.x(), m_provisionalDot.y()));
        const QRectF dotRect = QRectF((intersection - QPointF(dotImage.width() * 0.5, dotImage.height() * 0.5)), (intersection + QPointF(dotImage.width() * 0.5, dotImage.height() * 0.5)));

        QSGImageNode *dotNode = window()->createImageNode();
        dotNode->setRect(dotRect);
        QSGTexture *dotTexture = dotTextures[m_engine->currentPlayer()];
        dotNode->setTexture(dotTexture);
        provisionalDotContainerNode->appendChildNode(dotNode);
    }
}

void GameBoard::updateProvisionalChainContainerNode(GameBoard::QSGGameBoardNode *node)
{
    QSGOpacityNode *provisionalChainContainerNode = node->provisionalChainContainerNode();
    provisionalChainContainerNode->setOpacity(0.5);
    provisionalChainContainerNode->removeAllChildNodes();

    if (m_provisionalChain.size() > 1) {
        const std::deque<Dot> &chain = m_provisionalChain;
        Stroke *stroke = m_markStrokes[m_engine->currentPlayer()];
        QVector<QSGMaterial *> lineMaterials = node->lineMaterials();
        QSGMaterial *chainMaterial = lineMaterials[m_engine->currentPlayer()];
        const QTransform gridDisplayTransform = GameBoard::gridDisplayTransform();

        QSGGeometryNode *chainNode = new QSGGeometryNode();
        QSGGeometry *chainGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), static_cast<int>(chain.size()));
        chainGeometry->setLineWidth(static_cast<float>(stroke->width()));
        chainGeometry->setDrawingMode(QSGGeometry::DrawLineStrip);
        chainNode->setGeometry(chainGeometry);
        chainNode->setFlag(QSGNode::OwnsGeometry);
        // painter->setPen(QPen(stroke->color(), stroke->width(), Qt::DashLine, Qt::FlatCap));
        chainNode->setMaterial(chainMaterial);
        provisionalChainContainerNode->appendChildNode(chainNode);

        QSGGeometry::Point2D *vertices = chainGeometry->vertexDataAsPoint2D();
        size_t i = 0;

        for (const Dot &dot : chain) {
            const QPointF point = gridDisplayTransform.map(findIntersection(dot.x(), dot.y()));

            vertices[i++].set(static_cast<float>(point.x()), static_cast<float>(point.y()));
        }
    }
}

void GameBoard::prepareDotTextures(GameBoard::QSGGameBoardNode *node)
{
    if (!m_dotImagesDirty) {
        return;
    }

    QVector<QSGTexture *> dotTextures = node->dotTextures();

    for (const QSGTexture *dotTexture : dotTextures) {
        delete dotTexture;
    }
    dotTextures.clear();

    for (const QImage &dotImage : m_dotImages) {
        QSGTexture *dotTexture = window()->createTextureFromImage(dotImage);
        dotTextures.append(dotTexture);
    }

    node->setDotTextures(dotTextures);

    m_dotImagesDirty = false;
}

void GameBoard::prepareLineMaterials(GameBoard::QSGGameBoardNode *node)
{
    if (!m_lineMaterialsDirty) {
        return;
    }

    QVector<QSGMaterial *> lineMaterials = node->lineMaterials();

    for (const QSGMaterial *lineMaterial : lineMaterials) {
        delete lineMaterial;
    }
    lineMaterials.clear();

    for (const Stroke *stroke : m_markStrokes) {
        QSGFlatColorMaterial *lineMaterial = new QSGFlatColorMaterial();
        lineMaterial->setColor(stroke->color());
        lineMaterials.append(lineMaterial);
    }

    node->setLineMaterials(lineMaterials);

    m_lineMaterialsDirty = false;
}

QTransform GameBoard::gridDisplayTransform() const
{
    return QTransform().rotate(m_gridRotation);
}
