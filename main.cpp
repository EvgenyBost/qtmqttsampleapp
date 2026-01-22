#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "mosquittopublisher.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    MosquittoPublisher publisher;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("mqttPublisher", &publisher);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
