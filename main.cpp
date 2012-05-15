#include <QtGui/QApplication>
#include "mainwindow.h"
#include "QGraphicsScene"
#include "QLineF"
#include "QVarLengthArray"
#include "QPainter"
#include "QGraphicsView"


class GridScene : public QGraphicsScene
{
public:
    GridScene(qreal x, qreal y, qreal w, qreal h)
        : QGraphicsScene(x, y, w, h)
    { }

protected:
    void drawBackground(QPainter *painter, const QRectF &rect)
    {
        const int gridSize = 25;

        qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
        qreal top = int(rect.top()) - (int(rect.top()) % gridSize);

        QVarLengthArray<QLineF, 100> lines;

        for (qreal x = left; x < rect.right(); x += gridSize)
            lines.append(QLineF(x, rect.top(), x, rect.bottom()));
        for (qreal y = top; y < rect.bottom(); y += gridSize)
            lines.append(QLineF(rect.left(), y, rect.right(), y));

       // qDebug() << lines.size();

        painter->drawLines(lines.data(), lines.size());
    }
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    GridScene scene(-1000, -1000, 2000, 2000);
    QGraphicsView view(&scene);
    view.show();

    return app.exec();
}
