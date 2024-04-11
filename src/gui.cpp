#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QObject>
#include <QQmlContext>
#include <string>
#include <opencv2/core/core.hpp>

#include "backend.hpp"

int main(int argc, char *argv[]) {
    /*
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
	astrosight::Backend backend;
    QQmlImageProviderBase * preview = dynamic_cast<QQmlImageProviderBase*>(&backend);  
    engine.addImageProvider("preview", preview);
    engine.rootContext()->setContextProperty("Model", backend.model);
    engine.load("main.qml");
	QObject::connect(&engine, &QQmlApplicationEngine::quit, &QGuiApplication::quit);
    return app.exec();
    */
    
    astrosight::Backend backend;
    std::vector<std::string> folders = {"Light", "Dark", "Flat", "Dark_Flat", "Bias"};
    for (std::string folder : folders) {
        std::string full_pattern = "/media/ubuntu/512MicroSSD/test_data/cocoon/" + folder + "/*.CR2";
        backend.select_files(full_pattern);
    }
    /*
    std::string file = "L_0055_ISO800_240s__18C.CR2";
    if (std::optional<cv::Mat> frame = astrosight::load_frame(file)) {
        astrosight::display_frame(*frame);
    }
    */
    return 0;
}
