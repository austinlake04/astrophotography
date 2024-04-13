#ifndef ASTROSIGHT_BACKEND
#define ASTROSIGHT_BACKEND

#include <CCfits/CCfits>
#include "libraw/libraw.h"
#include <opencv2/opencv.hpp>
#include <QQuickImageProvider>
#include <QPixmap>

#include <valarray>
#include <string>
#include <vector>
#include <tuple>
#include <ctime>
#include <cstddef>
#include <iostream>
#include <optional>
#include <typeinfo>
#include <filesystem>
#include <glob.h>

namespace astrosight {

typedef enum image_type_t {
    light = 0,
    dark = 1,
    flat = 2,
    dark_flat = 3,
    bias = 4
} image_type;

typedef struct image_t {
    cv::Mat matrix;
    std::string file;
    astrosight::image_type type;
    std::time_t time;
    std::string make;
    std::string model;
    float iso;
    float shutter_speed;
    float aperture;
    float focal_length;
    bool stacked = false;
    bool calibrated = false;
    bool registered = false;
} image;

QImage create_qimage(const astrosight::image& frame, const bool thumbnail);

void display_frame(const astrosight::image& frame);

class Backend : public QQuickImageProvider {
public:
	Backend() : QQuickImageProvider(QQuickImageProvider::Image) {};

	QImage requestImage(
        const QString &id,
        QSize *size,
        const QSize &requestedSize
    ) override;

    void select_files(std::string pattern);

private:

    std::optional<astrosight::image> load_frame(const std::string file, const astrosight::image_type type);

    void generate_master_frames();

    void calibrate_frames();

    void register_frames();

    void set_preview(const astrosight::image& frame);

    std::vector<astrosight::image> light_frames;
    std::vector<astrosight::image> dark_frames;
    std::vector<astrosight::image> flat_frames;
    std::vector<astrosight::image> dark_flat_frames;
    std::vector<astrosight::image> bias_frames;    
    std::optional<astrosight::image> stacked_image;
    std::optional<astrosight::image> master_dark_frame;
    std::optional<astrosight::image> master_flat_frame;
    std::optional<astrosight::image> master_dark_flat_frame;
    std::optional<astrosight::image> master_bias_frame;
    int reference_index;
    astrosight::image const * preview;
    cv::Ptr<cv::Feature2D> detector = cv::ORB::create();
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::BFMatcher::create(cv::NORM_HAMMING, true);
};

}

#endif // ASTROSIGHT_BACKEND