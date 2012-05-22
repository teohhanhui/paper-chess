#include <QApplication>
#include <QtDeclarative>
#include "qmlapplicationviewer.h"
#include "stroke.h"
#include "gamegrid.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    qmlRegisterType<Stroke>("CustomComponents", 1, 0, "Stroke");
    qmlRegisterType<GameGrid>("CustomComponents", 1, 0, "GameGrid");

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/PaperChess/main.qml"));
    viewer.showExpanded();

    return app->exec();
}
