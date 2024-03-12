#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QObject>
#include <QQmlContext>
#include "backend.cpp"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
	astrosight::Backend backend;
	QQmlImageProviderBase * preview = dynamic_cast<QQmlImageProviderBase*>(&backend);  
    engine.addImageProvider("preview", preview);
    //engine.rootContext()->setContextProperty("Model", backend.model);
    engine.load("main.qml");
	QObject::connect(&engine, &QQmlApplicationEngine::quit, &QGuiApplication::quit);
    return app.exec();
}
