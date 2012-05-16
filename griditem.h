#ifndef GRIDITEM_H
#define GRIDITEM_H

#include <QDeclarativeItem>

class Stroke;

class GridItem : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(int rows READ rows WRITE setRows)
    Q_PROPERTY(int columns READ columns WRITE setColumns)
    Q_PROPERTY(Stroke *stroke READ stroke)

public:
    explicit GridItem(QDeclarativeItem *parent = 0);
    ~GridItem();

    int rows() const;
    void setRows(int rows);

    int columns() const;
    void setColumns(int columns);

    Stroke *stroke() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

signals:

public slots:
    void strokeLines();

private:
    int m_rows;
    int m_columns;
    Stroke *m_stroke;
};

#endif // GRIDITEM_H
