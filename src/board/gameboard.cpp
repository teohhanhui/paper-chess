#include "gameboard.h"
#include <algorithm>
#include <QDeclarativeProperty>
#include "gamegrid.h"

using namespace std;

GameBoard::GameBoard(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_rows(1)
    , m_columns(1)
    , m_grid(new GameGrid(1, 1))
{
    m_grid->setParentItem(this);

    // Set the grid to fill the board
    // QML equivalent: "anchors.fill: parent"
    QDeclarativeProperty prop(m_grid, "anchors.fill");
    prop.write(QVariant::fromValue<QDeclarativeItem *>(this));

    connect(this, SIGNAL(widthChanged()), SLOT(resizeGrid()));
}

int GameBoard::rows() const
{
    return m_rows;
}

void GameBoard::setRows(int rows)
{
    if (rows != m_rows) {
        m_rows = rows;
        resizeGrid();
    }
}

int GameBoard::columns() const
{
    return m_columns;
}

void GameBoard::setColumns(int columns)
{
    if (columns != m_columns) {
        m_columns = columns;
        resizeGrid();
    }
}

Stroke *GameBoard::gridStroke() const
{
    return m_grid->stroke();
}

void GameBoard::resizeGrid()
{
    int shorter = min(m_rows, m_columns);
    int taller = max(m_rows, m_columns);

    if (width() < height()) {
        m_grid->setRows(taller);
        m_grid->setColumns(shorter);
    }
    else {
        m_grid->setRows(shorter);
        m_grid->setColumns(taller);
    }
}
