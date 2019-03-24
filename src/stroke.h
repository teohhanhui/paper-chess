#ifndef STROKE_H
#define STROKE_H

#include <QObject>
#include <QtGui/QColor>

class Stroke : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)

public:
    explicit Stroke(QObject *parent = nullptr);

    QColor color() const;
    void setColor(const QColor &color);

    qreal width() const;
    void setWidth(qreal width);

signals:
    void colorChanged();
    void widthChanged();

private:
    QColor m_color;
    qreal m_width;
};

#endif // STROKE_H
