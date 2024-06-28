#ifndef ASTROSIGHT_BACKEND
#define ASTROSIGHT_BACKEND

#include <CCfits/CCfits>
#include "libraw/libraw.h"
#include <opencv2/opencv.hpp>

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

typedef enum {
    monochrome = 0,
    colored = 1,
} color_mode_t;

typedef enum {
    light = 0,
    dark = 1,
    flat = 2,
    dark_flat = 3,
    bias = 4,
    blue = 5,
    green = 6,
    red = 7
} image_type_t;

typedef struct {
    Mat frame;
    string file;
    bool calibrated = false;
    optional<vector<KeyPoint>> keypoints;
} image_t;
/*
    image_type_t type;
    time_t time;
    string make;
    string model;
    float iso;
    float shutter_speed;
    float aperture;
    float focal_length;
    bool stacked = false;
*/

vector<string> select_files(string pattern);

class Backend {
public:
	Backend() {};
    /*
	QImage requestImage(
        const QString &id,
        QSize *size,
        const QSize &requestedSize
    ) override;
    QImage create_qimage(const image_t&  frame, const bool thumbnail);
    */
    void load_frames(vector<string>& file, image_type_t type);
    void create_master_frames();
    void calibrate_frames();
    void create_rgb_frames();
    void register_frames();
    void stack_frames();
    void set_preview(image_t&  frame);

    bool quiet = false;
    optional<image_t> stacked_image;
    vector<image_t> light_images;

private:

    vector<image_t> blue_images;
    vector<image_t> green_images;
    vector<image_t> red_images;
    vector<image_t> dark_images;
    vector<image_t> flat_images;
    vector<image_t> dark_flat_images;
    vector<image_t> bias_images;    
    optional<image_t> master_dark_image;
    optional<image_t> master_flat_image;
    optional<image_t> master_dark_flat_image;
    optional<image_t> master_bias_image;
    std::shared_ptr<image_t> reference_image;
    std::shared_ptr<image_t> preview_image;
    cv::Ptr<cv::Feature2D> detector = cv::ORB::create();
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::BFMatcher::create(
        cv::NORM_HAMMING, 
        true
    );
    color_mode_t color_mode = color_mode_t::colored;
    int demosaic_algorithm = cv::COLOR_BayerBG2BGR;
};

void display_frame(const image_t& frame);

}

#endif // ASTROSIGHT_BACKEND
