#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QObject>
#include <QQmlContext>
#include <string>
#include <argparse/argparse.hpp>

#include "backend.hpp"

using astrosight::image_type;

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
    argparse::ArgumentParser args("program_name");

    args.add_argument("-q", "--quiet")
        .help("quiet terminal output")
        .flag();

    try {
        args.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        cerr << err.what() << endl;
        cerr << args;
        return 1;
    }
    backend.quiet = args.get<bool>("--quiet");

    
    const array<string, 5> folders = { "light", "dark", "flat", "bias" };
    const vector<image_type> types = {
        image_type::light,
        image_type::dark,
        image_type::flat,
        image_type::bias
    };
    
    time_point<steady_clock> start, stop;
    std::chrono::duration<double, std::milli> duration_ms;
    
    for (const image_type& type : types) {
        string full_pattern = "/media/ubuntu/512MicroSSD/test_data/cocoon/" + folders[static_cast<int>(type)] + "/*.CR2";
        vector<string> files = astrosight::select_files(full_pattern);
        
        start = steady_clock::now(); 
        backend.load_frames(files, type);
        duration_ms = steady_clock::now() - start; 
        if (!quiet) {
            cout << "time (ms) to load " << files.size() << " " << folders[static_cast<int>(type)] << " frames:\t" << duration_ms.count() << endl;
        }
    };
    return 0;

    start = steady_clock::now(); 
    backend.generate_master_frames(); 
    duration_ms = steady_clock::now() - start; 
    if (!quiet) {
        cout << "time (ms) to generate master frames:\t" << duration_ms.count() << endl;
    }

    start = steady_clock::now(); 
    backend.calibrate_frames();
    duration_ms = steady_clock::now() - start; 
    if (!quiet) {
        cout << "time (ms) to calibrate " << backend.light_frames.size() << " light frames:\t" << duration_ms.count() << endl;
    }
    
    start = steady_clock::now(); 
    backend.create_rgb_frames(); 
    duration_ms = steady_clock::now() - start; 
    if (!quiet) {
        cout << "time (ms) to demosaic " << backend.light_frames.size() << " light frames:\t" << duration_ms.count() << endl;
    } 

    start = steady_clock::now(); 
    backend.register_frames();
    duration_ms = steady_clock::now() - start; 
    if (!quiet) {
        cout << "time (ms) to register " << backend.light_frames.size() << " light frames:\t" << duration_ms.count() << endl;
    }
    
    start = steady_clock::now(); 
    backend.stack_frames();
    duration_ms = steady_clock::now() - start; 
    if (!quiet) {
        cout << "time (ms) to register " << backend.light_frames.size() << " light frames:\t" << duration_ms.count() << endl;
    }

    /*
    average(backend.stacked_image, backend.light_frames); 
    if (backend.stacked_image) {
        astrosight::display_frame(*backend.stacked_image);
    }
    */
    

    /*
    string file = "L_0055_ISO800_240s__18C.CR2";
    if (optional<Mat> frame = astrosight::load_frame(file)) {
        astrosight::display_frame(*frame);
    }
    */
    return 0;
}
