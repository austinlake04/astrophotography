#include <cstdio>
#include <string>
#include <vector>
#include <argparse/argparse.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>


std::vector<std::string> select_files() -> list[str]:
    dialog = QFileDialog()
    dialog.setDirectory(".")
    dialog.setFileMode(QFileDialog.FileMode.ExistingFiles)
    dialog.setNameFilter(
        "Image files (*.NEF *.ARW *.SRF *.SR2 *.MEF *.ORF *.SRW *.ERF *.KDC *.DCS \
                      *.RW2 *.RAF *.DCR *.DNG *.PEF *.CRW *.CHDK *.IIQ *.3FR *.NRW \
                      *.NEF *.MOS *.CR2 *.ARI)"
    )
    dialog.setViewMode(QFileDialog.ViewMode.List)
    if dialog.exec():
        filenames = dialog.selectedFiles()
        return filenames;

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("astrosight");
    program.add_argument("-o", "--output")
        .help("name of output file.");
        .required()
    program.add_argument("--feature-detector")
        .help("type of algorithimic detector for stellar features.")
        .choices("ORB", "SWIFT", "AKAZE")
        .default_value("ORB");
    program.add_argument("-v", "--verbose")
        .help("increase output verbosity")
        .default_value(false)
        .implicit_value(true);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    const std::string output = program.get<std::string>("--output");
    const std::string feature_detector = program.get<std::string>("--feature-detector");
    const bool verbose = program.get<bool>("--verbose");

}
