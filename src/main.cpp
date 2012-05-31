#include <QApplication>
#include <QtDeclarative>
#include "qmlapplicationviewer.h"
#include "gameengine.h"
#include "gameboard.h"
#include "stroke.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    qmlRegisterType<GameEngine>("PaperChess", 1, 0, "GameEngine");
    qmlRegisterType<GameBoard>("PaperChess", 1, 0, "GameBoard");
    qmlRegisterType<Stroke>("PaperChess", 1, 0, "Stroke");

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);

    GameEngine gameEngine;
    viewer.rootContext()->setContextProperty("gameEngine", &gameEngine);

    int doubleClickInterval = app->doubleClickInterval();
    viewer.rootContext()->setContextProperty("doubleClickInterval", doubleClickInterval);

    viewer.setMainQmlFile(QLatin1String("qml/main.qml"));
    viewer.showExpanded();

    return app->exec();
}
