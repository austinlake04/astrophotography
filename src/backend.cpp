#include "backend.hpp"

QImage astrosight::create_qimage(
    const astrosight::image& frame,
    const bool thumbnail
) {
    QImage::Format format;
    switch (frame.matrix.type()) {
        case CV_16UC1:
            format = QImage::Format_Grayscale16;
            break;
        case CV_16UC3:
            format = QImage::Format_RGBX64;
            break;
        case CV_16UC4:
            format = QImage::Format_RGBA64;
            break;
        case CV_8UC1:
            format = QImage::Format_Grayscale8;
            break;
        case CV_8UC3:
            format = QImage::Format_RGB888;
            break;
        case CV_8UC4:
            format = QImage::Format_RGBA8888;
            break;
        case CV_32FC3:
            format = QImage::Format_RGBX32FPx4;
            break;
        case CV_32FC4:
            format = QImage::Format_RGBA32FPx4;
            break;
        default:
            format = QImage::Format_Invalid;
            break;
    }

    return QImage((uchar*) frame.matrix.data, frame.matrix.cols, frame.matrix.rows, frame.matrix.step, QImage::Format_Grayscale16).rgbSwapped();
}

void astrosight::display_frame(
    const astrosight::image& frame
) {
    cv::Mat downsized_frame;
    cv::resize(frame.matrix, downsized_frame, cv::Size(), 0.25, 0.25, cv::INTER_AREA);
    std::string window_name = "Preview Window";
    cv::namedWindow(window_name, cv::WINDOW_NORMAL);
    cv::imshow(window_name, downsized_frame);
    cv::waitKey(0);
    cv::destroyAllWindows();
}

void astrosight::Backend::select_files(
    const std::string pattern
) {
    glob_t globbuf;
    int err = glob(pattern.c_str(), 0, NULL, &globbuf);
    if(err == 0)
    {
        for (size_t i = 0; i < globbuf.gl_pathc; i++)
        {
            printf("%s\n", globbuf.gl_pathv[i]);
        }

        globfree(&globbuf);
    }
}

QImage astrosight::Backend::requestImage(
    const QString &id,
    QSize *size,
    const QSize &requestedSize
) {
    QImage qimage;
    return qimage;
}

std::optional<astrosight::image> astrosight::Backend::load_frame(
    const std::string file,
    const astrosight::image_type type
) {
    std::optional<astrosight::image> result;
    std::vector<std::string> fits_files = {".fits", ".fts", ".fit"};
    if (std::find(fits_files.begin(), fits_files.end(), std::filesystem::path(file).extension()) != fits_files.end()) {
        CCfits::FITS fits(file, CCfits::Read, true);
        CCfits::PHDU& raw = fits.pHDU(); 
        raw.readAllKeys();
        std::valarray<std::uint16_t> contents;
        raw.read(contents);
        std::size_t height = raw.axis(0);
        std::size_t width = raw.axis(1);
        cv::Mat frame(height, width, CV_16UC1);
        std::memcpy(frame.data, static_cast<void*>(&contents), height*width*sizeof(std::uint16_t));
        result = (astrosight::image) {
            .matrix = frame,
            .file = file,
            .type = type,
            .time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()),
            .make = "",
            .model = "", 
            .iso = 0,
            .shutter_speed = 0,
            .aperture = 0,
            .focal_length = 0,
        };
    } else {
        LibRaw raw;
        if (raw.open_file(file.c_str()) == LIBRAW_SUCCESS) {
            raw.unpack();
            std::size_t height = raw.imgdata.sizes.raw_height;
            std::size_t width = raw.imgdata.sizes.raw_width;     
            cv::Mat frame(height, width, CV_16UC1);
            std::memcpy(frame.data, raw.imgdata.rawdata.raw_image, height*width*sizeof(std::uint16_t));
            // Some RAW images have a frame of pixel that line the top and left of the image
            // This must be cropped out
            cv::Rect crop_window(raw.imgdata.sizes.left_margin, raw.imgdata.sizes.top_margin, raw.imgdata.sizes.width, raw.imgdata.sizes.height);
            result = (astrosight::image) {
                .matrix = frame(crop_window),
                .file = file,
                .type = type,
                .time = raw.imgdata.other.timestamp,
                .make = raw.imgdata.idata.make,
                .model = raw.imgdata.idata.model,
                .iso = raw.imgdata.other.iso_speed,
                .shutter_speed = raw.imgdata.other.shutter,
                .aperture = raw.imgdata.other.aperture,
                .focal_length = raw.imgdata.other.focal_len,
            };
        }
        raw.recycle();
    }
    return result;
}

void astrosight::Backend::calibrate_frames() {
    auto calibration = [this](astrosight::image& light_frame) {
        if (!light_frame.calibrated) {
            if (this->master_dark_frame) {
                cv::subtract(light_frame.matrix, (*this->master_dark_frame).matrix, light_frame.matrix, CV_32FC1);
            }
            if (this->master_flat_frame) {
                cv::divide(light_frame.matrix, (*this->master_flat_frame).matrix, light_frame.matrix, CV_32FC1); 
            }
            light_frame.calibrated = true;
        }
    };
    std::for_each(this->light_frames.begin(), this->light_frames.end(), calibration);
}

void astrosight::Backend::register_frames() {
    std::vector<cv::KeyPoint> reference_keypoints;
    cv::Mat reference_descriptors;
    this->detector->detectAndCompute(this->light_frames[this->reference_index].matrix, cv::Mat(), reference_keypoints, reference_descriptors);

    for (std::size_t i = 0; i < this->light_frames.size(); ++i) {
        if (i != this->reference_index) {
            std::vector<cv::KeyPoint> keypoints;
            cv::Mat descriptors;
            this->detector->detectAndCompute(this->light_frames[i].matrix, cv::Mat(),  keypoints, descriptors);

            std::vector<cv::DMatch> matches;
            this->matcher->match(descriptors, reference_descriptors, matches);
            std::sort(
                matches.begin(),
                matches.end(),
                [](cv::DMatch& a, cv::DMatch& b) { return a.distance > b.distance; }
            );
            std::vector<cv::Point2f> points, reference_points;
            std::transform(
                matches.begin(),
                matches.end(),
                std::back_inserter(points),
                [&keypoints](const cv::DMatch& match){ return keypoints[match.queryIdx].pt; });
            std::transform(
                matches.begin(), 
                matches.end(), 
                std::back_inserter(reference_points),
                [&reference_keypoints](const cv::DMatch& match){ return reference_keypoints[match.trainIdx].pt; });

            try {
                cv::Mat homography;
                cv::findHomography(points, reference_points, homography, cv::RANSAC);
                cv::warpPerspective(this->light_frames[i].matrix, this->light_frames[i].matrix, homography, this->light_frames[i].matrix.size());
            } catch (cv::Exception) {
                std::cout << "too few stars detected" << std::endl;
            }
        }
        this->light_frames[i].registered = true; 
    }
}

void astrosight::Backend::generate_master_frames() {
    
    auto average = [](std::optional<astrosight::image>& master_frame, const std::vector<astrosight::image>& frames) {
        cv::Mat master_matrix = std::accumulate(
            std::next(frames.begin()),
            frames.end(),
            frames[0].matrix,
            [](cv::Mat& summed_matrix,const astrosight::image& next_frame){ return summed_matrix + next_frame.matrix; }
        );

        master_frame = (astrosight::image) {
            .matrix = master_matrix/frames.size(),
            .file = "", 
            .type = frames[0].type,
            .time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()),
            .make = frames[0].make,
            .model = frames[0].model,
            .iso = frames[0].iso,
            .shutter_speed = frames[0].shutter_speed,
            .aperture = frames[0].aperture,
            .focal_length = frames[0].focal_length,
        };
    };

    if (this->dark_frames.size()) {
        average(this->master_dark_frame, this->dark_frames);
    }

    if (this->flat_frames.size()) {
        average(this->master_flat_frame, this->flat_frames);
    }

    if (this->dark_flat_frames.size()) {
        average(this->master_dark_flat_frame, this->dark_flat_frames);
    }

    if (this->bias_frames.size()) {
        average(this->master_bias_frame, this->bias_frames);
    }
}

void astrosight::Backend::set_preview(
    const astrosight::image& frame
) {
    this->preview = &frame;
}