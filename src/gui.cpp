#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>

int main(int argc, char *argv[]) {
	QGuiApplication app(argc, argv);
	QQmlApplicationEngine engine;
	engine.load(QUrl::fromLocalFile("main.qml"));
	return app.exec();
}
