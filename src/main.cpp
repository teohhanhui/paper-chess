#include "gameboard.h"
#include "gameengine.h"
#include "stroke.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<GameEngine>("PaperChess", 1, 0, "GameEngine");
    qmlRegisterType<GameBoard>("PaperChess", 1, 0, "GameBoard");
    qmlRegisterType<Stroke>("PaperChess", 1, 0, "Stroke");

    QQmlApplicationEngine engine;

    GameEngine gameEngine;
    engine.rootContext()->setContextProperty("gameEngine", &gameEngine);

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
