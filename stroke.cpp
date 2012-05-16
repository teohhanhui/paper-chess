#include "stroke.h"

Stroke::Stroke(QObject *parent)
    : QObject(parent)
    , m_color(Qt::black)
    , m_width(1)
{
}

QColor Stroke::color() const
{
    return m_color;
}

void Stroke::setColor(const QColor &color)
{
    if (color != m_color) {
        m_color = color;
        emit colorChanged();
    }
}

qreal Stroke::width() const
{
    return m_width;
}

void Stroke::setWidth(const qreal width)
{
    if (width != m_width) {
        m_width = width;
        emit widthChanged();
    }
}
