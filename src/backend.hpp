#ifndef ASTROSIGHT_BACKEND
#define ASTROSIGHT_BACKEND

#include <CCfits/CCfits>
#include "libraw/libraw.h"
#include <opencv2/opencv.hpp>
#include <QQuickImageProvider>
#include <QPixmap>


#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <execution>
#include <filesystem>
#include <iostream>
#include <optional>
#include <ranges>
#include <string>
#include <valarray>
#include <vector>

#include <glob.h>

using cv::Mat;
using cv::KeyPoint;
using std::array;
using std::cerr;
using std::chrono::time_point;
using std::chrono::system_clock;
using std::chrono::steady_clock;
using std::cout;
using std::endl;
using std::execution::par_unseq;
using std::for_each;
using std::move;
using std::make_shared;
using std::optional;
using std::size_t;
using std::string;
using std::uint16_t;
using std::vector;

namespace astrosight {

typedef enum color_mode_t {
    monochrome = 0,
    colored = 1,
} color_mode;

typedef enum image_type_t {
    light = 0,
    dark = 1,
    flat = 2,
    dark_flat = 3,
    bias = 4,
    blue = 5,
    green = 6,
    red = 7
} image_type;

typedef struct image_t {
    Mat matrix;
    string file;
    image_type type;
    time_t time;
    string make;
    string model;
    float iso;
    float shutter_speed;
    float aperture;
    float focal_length;
    bool stacked = false;
    bool calibrated = false;
    optional<vector<KeyPoint>> keypoints;
} image;

vector<string> select_files(string pattern);

class Backend : public QQuickImageProvider {
public:
	Backend() : QQuickImageProvider(QQuickImageProvider::Image) {};

	QImage requestImage(
        const QString &id,
        QSize *size,
        const QSize &requestedSize
    ) override;

    
    QImage create_qimage(const image& frame, const bool thumbnail);

    void load_frames(vector<string>& file, image_type type);

    void create_master_frames();

    void calibrate_frames();

    void create_rgb_frames();

    void register_frames();

    void stack_frames();

    void set_preview(image& frame);

    void display_frame(const image& frame);

    bool quiet = false;

private:

    vector<image> light_frames;
    vector<image> blue_frames;
    vector<image> green_frames;
    vector<image> red_frames;
    vector<image> dark_frames;
    vector<image> flat_frames;
    vector<image> dark_flat_frames;
    vector<image> bias_frames;    
    optional<image> stacked_image;
    optional<image> master_dark_frame;
    optional<image> master_flat_frame;
    optional<image> master_dark_flat_frame;
    optional<image> master_bias_frame;
    std::shared_ptr<image> reference_frame;
    std::shared_ptr<image> preview_frame;
    cv::Ptr<cv::Feature2D> detector = cv::ORB::create();
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::BFMatcher::create(
        cv::NORM_HAMMING, 
        true
    );
    color_mode mode = color_mode::colored;
    int demosaic_algorithm = cv::COLOR_BayerBG2BGR;
};

}

#endif // ASTROSIGHT_BACKEND