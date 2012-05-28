#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QDeclarativeItem>

class GameGrid;
class Stroke;

class GameBoard : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(int rows READ rows WRITE setRows)
    Q_PROPERTY(int columns READ columns WRITE setColumns)
    Q_PROPERTY(Stroke *gridStroke READ gridStroke)

public:
    explicit GameBoard(QDeclarativeItem *parent = 0);

    int rows() const;
    void setRows(int rows);

    int columns() const;
    void setColumns(int columns);

    Stroke *gridStroke() const;

signals:
    
public slots:
    
protected slots:
    void resizeGrid();

private:
    int m_rows;
    int m_columns;
    GameGrid *m_grid;
};

#endif // GAMEBOARD_H
