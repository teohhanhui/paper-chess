#ifndef GAMEGRID_H
#define GAMEGRID_H

#include <QDeclarativeItem>

class Stroke;

class GameGrid : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(int rows READ rows WRITE setRows)
    Q_PROPERTY(int columns READ columns WRITE setColumns)
    Q_PROPERTY(Stroke *stroke READ stroke)

public:
    explicit GameGrid(QDeclarativeItem *parent = 0);
    ~GameGrid();

    int rows() const;
    void setRows(int rows);

    int columns() const;
    void setColumns(int columns);

    Stroke *stroke() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

public slots:
    void drawGrid();
    void drawChain();

signals:

private:
    int m_rows;
    int m_columns;
    Stroke *m_stroke;
};

#endif // GAMEGRID_H
