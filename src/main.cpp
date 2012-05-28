#include <QApplication>
#include <QtDeclarative>
#include "qmlapplicationviewer.h"
#include "stroke.h"
#include "board/gameboard.h"
#include "gameengine.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    qmlRegisterType<Stroke>("CustomComponents", 1, 0, "Stroke");
    qmlRegisterType<GameBoard>("CustomComponents", 1, 0, "GameBoard");

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);

    GameEngine gameEngine;
    viewer.rootContext()->setContextProperty("gameEngine", &gameEngine);

    viewer.setMainQmlFile(QLatin1String("qml/main.qml"));
    viewer.showExpanded();

    return app->exec();
}
