#include <QApplication>
#include <QtDeclarative>
#include "qmlapplicationviewer.h"
#include "stroke.h"
#include "griditem.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    qmlRegisterType<Stroke>("CustomComponents", 1, 0, "Stroke");
    qmlRegisterType<GridItem>("CustomComponents", 1, 0, "GridItem");

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/PaperChess/main.qml"));
    viewer.showExpanded();

    return app->exec();
}
