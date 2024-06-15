#include "backend.hpp"

vector<string> astrosight::select_files(const string pattern) {
    glob_t globbuf;
    if (glob(pattern.c_str(), 0, NULL, &globbuf) == 0) {
        vector<string> files(
            globbuf.gl_pathv,
            globbuf.gl_pathv + globbuf.gl_pathc
        );
        globfree(&globbuf);
        return files;
    }
    return {};
}

/*
QImage astrosight::Backend::requestImage(
    const QString &id,
    QSize *size,
    const QSize &requestedSize
) {
    QImage qimage;
    return qimage;
}

QImage astrosight::Backend::create_qimage(const image& frame, const bool thumbnail) {
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

    return QImage(
        (uchar*) frame.matrix.data,
        frame.matrix.cols,
        frame.matrix.rows,
        frame.matrix.step,
        QImage::Format_Grayscale16
    ).rgbSwapped();
}
*/

void astrosight::Backend::load_frames(vector<string>& files, image_type type) {
    vector<image>& frames = light_frames;
    switch (type) {
        case image_type::dark:
            frames = dark_frames;
            break;       
        case image_type::flat:
            frames = flat_frames;
            break;       
        case image_type::dark_flat:
            frames = dark_flat_frames;
            break;       
        case image_type::bias:
            frames = bias_frames;
            break;
        case image_type::blue:
            frames = blue_frames;
            break;
        case image_type::green:
            frames = green_frames;
            break;
        case image_type::red:
            frames = red_frames;
            break;
        default:
            break;
    }
    for (string file : files) {
        if (std::find_if(frames.begin(), frames.end(), [&file](const image& frame) { return frame.file == file; }) == frames.end()) {
            const array<string, 3> fits_files = {".fits", ".fts", ".fit"};
            if (std::find(fits_files.begin(), fits_files.end(), std::filesystem::path(file).extension()) != fits_files.end()) {
                CCfits::FITS fits(file, CCfits::Read, true);
                CCfits::PHDU& raw = fits.pHDU(); 
                raw.readAllKeys();
                std::valarray<uint16_t> contents;
                raw.read(contents);
                size_t height = raw.axis(0);
                size_t width = raw.axis(1);
                Mat frame(height, width, CV_16UC1);
                std::memmove(
                    frame.data,
                    static_cast<void*>(&contents),
                    height*width*sizeof(uint16_t)
                );
                frames.push_back((astrosight::image) {
                        move(frame),
                        move(file),
                        move(type),
                        system_clock::to_time_t(system_clock::now()),
                        "",
                        "", 
                        0,
                        0,
                        0,
                        0
                });
            } else {
                LibRaw raw;
                if (raw.open_file(file.c_str()) == LIBRAW_SUCCESS) {
                    raw.unpack();
                    size_t raw_height = raw.imgdata.sizes.raw_height;
                    size_t raw_width = raw.imgdata.sizes.raw_width;
                    Mat frame(raw_height, raw_width, CV_16UC1);
                    std::memmove(
                        frame.data,
                        raw.imgdata.rawdata.raw_image,
                        raw_height*raw_width*sizeof(uint16_t)
                    );
                    /* Some RAW images have a frame of pixel that line the top and
                    * left of the image that must be cropped out.
                    */
                    frames.push_back((astrosight::image) {
                        move(frame(cv::Rect(
                            raw.imgdata.sizes.left_margin,
                            raw.imgdata.sizes.top_margin,
                            raw.imgdata.sizes.width,
                            raw.imgdata.sizes.height
                        ))),
                        move(file),
                        move(type),
                        move(raw.imgdata.other.timestamp),
                        move(raw.imgdata.idata.make),
                        move(raw.imgdata.idata.model),
                        move(raw.imgdata.other.iso_speed),
                        move(raw.imgdata.other.shutter),
                        move(raw.imgdata.other.aperture),
                        move(raw.imgdata.other.focal_len)
                    });
                }
                raw.recycle();
            }
        }
    }
}

void astrosight::Backend::calibrate_frames() {
    auto calibration = [this](vector<image>& frames){
        for (image& frame : frames ) {
            if (!frame.calibrated) {
                if (this->master_dark_frame) {
                    frame.matrix = 1.0 * frame.matrix - (*this->master_dark_frame).matrix;
                }
                if (this->master_flat_frame) {
                    if (this->master_dark_flat_frame) {
                        frame.matrix = 1.0 * frame.matrix / 
                            ((*this->master_flat_frame).matrix - (*this->master_dark_flat_frame).matrix);
                    } else if (this->master_bias_frame) {
                        frame.matrix = 1.0 * frame.matrix /
                            ((*this->master_flat_frame).matrix - (*this->master_bias_frame).matrix);
                    } else {
                        frame.matrix = 1.0 * frame.matrix /
                            (*this->master_flat_frame).matrix;
                    }
                }
                frame.calibrated = true;
            }
        }
    };

    switch (mode) {
        case color_mode::colored:
            calibration(light_frames);
            break;
        case color_mode::monochrome:
            calibration(red_frames);
            calibration(green_frames);
            calibration(blue_frames);
            break;
        default:
            break;
    }

}

void astrosight::Backend::create_rgb_frames() {
    switch (mode) {
        case color_mode::colored:
            for (image& light_frame : light_frames) {
                if (light_frame.matrix.channels() == 1) {
                    cv::demosaicing(
                        light_frame.matrix,
                        light_frame.matrix,
                        this->demosaic_algorithm
                    );
                }
            };
            break;
        /*
        case color_mode::monochrome:
            if (blue_frames.size() == green_frames.size() == red_frames.size()) {
                light_frames.clear();
                for (auto& [blue_frame, green_frame, red_frame] :
		    std::ranges::zip_view(blue_frames, green_frames, red_frames)
                ) {
                    Mat matrix = cv::merge(
                        blue_frame.matrix,
                        green_frame.matrix,
                        red_frame.matrix
                    );
                    light_frames.emplace_back(
                        matrix,
                        "", 
                        image_type::light,
                        system_clock::to_time_t(system_clock::now()),
                        blue_frame.make,
                        blue_frame.model,
                        blue_frame.iso,
                        blue_frame.shutter_speed,
                        blue_frame.aperture,
                        blue_frame.focal_length,
                        blue_frame.stacked,
                        blue_frame.calibrated,
                        blue_frame.keypoints
                    );
                } 
            }
            break;
        */
        default:
            break;
    }

}

void astrosight::Backend::register_frames() {
    if (!reference_frame) {
        if (!quiet) { cout << "No reference frame selected. Selecting for you." << endl; }
        switch (mode) {
            case color_mode::colored:
                if (light_frames.size()) { reference_frame = make_shared<image>(light_frames[0]); }
                else {
                    cerr << "No frames loaded. Unable to auto-select a reference frame." << endl;
                    return;
                }
                break;
            case color_mode::monochrome:
                if (blue_frames.size()) { reference_frame = make_shared<image>(blue_frames[0]); }
                else if (red_frames.size()) { reference_frame = make_shared<image>(green_frames[0]); }
                else if (red_frames.size()) { reference_frame = make_shared<image>(red_frames[0]); }
                else {
                    cerr << "No frames loaded. Unable to auto-select a reference frame." << endl;
                    return;
                }
                break;
            default:
                break;
        }
    }
    vector<KeyPoint> reference_keypoints;
    Mat reference_descriptors;
    detector->detectAndCompute(
        reference_frame->matrix,
        cv::noArray(),
        reference_keypoints,
        reference_descriptors
    );
    reference_frame->keypoints = reference_keypoints;

    for (image& light_frame : light_frames) {
        vector<KeyPoint> keypoints;
        if (reference_frame.get() == &light_frame) {    
            Mat descriptors;
            this->detector->detectAndCompute(
                light_frame.matrix,
                cv::noArray(),
                keypoints,
                descriptors
            );
            
            vector<cv::DMatch> matches;
            this->matcher->match(
                descriptors,
                reference_descriptors,
                matches
            );
            
            std::sort(
                matches.begin(),
                matches.end(),
                [](const cv::DMatch& a, const cv::DMatch& b) {
                    return a.distance > b.distance;
                }
            );

            vector<cv::Point2f> points, reference_points;
            points.reserve(matches.size());
            reference_points.reserve(matches.size());

            for (cv::DMatch& match : matches) {
                reference_points.emplace_back(
                    reference_keypoints[match.queryIdx].pt
                );
                points.emplace_back(
                    keypoints[match.queryIdx].pt
                );
            }
            

            try {
                const cv::Mat homography = cv::findHomography(
                    points,
                    reference_points,
                    cv::RANSAC
                );
                cv::warpPerspective(
                    light_frame.matrix,
                    light_frame.matrix,
                    homography,
                    light_frame.matrix.size()
                );
                // cv::perspectiveTransform(keypoints, keypoints, homography);
            } catch (const std::exception& err) {
                cerr << err.what() << endl;
            }
        }
        light_frame.keypoints = keypoints;
    }
}

void astrosight::Backend::create_master_frames() {
    auto average = [](
        const vector<image>& frames
    ) {
        optional<image> master_frame;
        if (frames.size()) {
            Mat master_matrix = std::accumulate(
                frames.begin(),
                frames.end(),
                Mat(),
                [](Mat master_matrix, const image& frame){
                    return master_matrix + frame.matrix;
                }
            )/frames.size();

            master_frame = (image) {
                .matrix = move(master_matrix),
                .file = "", 
                .type = frames[0].type,
                .time = system_clock::to_time_t(system_clock::now()),
                .make = frames[0].make,
                .model = frames[0].model,
                .iso = frames[0].iso,
                .shutter_speed = frames[0].shutter_speed,
                .aperture = frames[0].aperture,
                .focal_length = frames[0].focal_length,
                .stacked = true,
                .calibrated = false,
                .keypoints = std::nullopt
            };
        }

        return master_frame;
    };

    master_dark_frame = average(dark_frames);
    master_flat_frame = average(flat_frames);
    master_dark_flat_frame = average(dark_flat_frames);
    master_bias_frame = average(bias_frames);
}

void astrosight::Backend::stack_frames() {
    optional<image> master_frame;
    if (light_frames.size()) {
        /*
        Mat master_matrix = std::accumulate(
            light_frames.begin(),
            light_frames.end(),
            Mat(),
            [](Mat master_matrix, const astrosight::image& frame){
                return master_matrix + frame.matrix;
            }
        )/light_frames.size();
        */
        
        Mat master_matrix(light_frames[0].matrix.size(), CV_16UC3);
        for (image& frame: light_frames) { master_matrix += 1.0 * frame.matrix; }
        
        master_frame = (image) {
            .matrix = move(master_matrix),
            .file = "", 
            .type = image_type::light,
            .time = system_clock::to_time_t(system_clock::now()),
            .make = light_frames[0].make,
            .model = light_frames[0].model,
            .iso = light_frames[0].iso,
            .shutter_speed = light_frames[0].shutter_speed,
            .aperture = light_frames[0].aperture,
            .focal_length = light_frames[0].focal_length,
            .stacked = true,
            .calibrated = light_frames[0].calibrated,
            .keypoints = light_frames[0].keypoints
        };
    }

    stacked_image = master_frame;
}

void astrosight::Backend::set_preview(image& frame) { preview_frame = std::make_shared<image>(frame); }

void astrosight::display_frame(const image& frame) {
    if (!frame.matrix.empty()) {
        Mat downsized_frame;
        cv::resize(
            frame.matrix,
            downsized_frame,
            cv::Size(),
            0.25,
            0.25,
            cv::INTER_AREA
        );
        string window_name = "Preview Window";
        cv::namedWindow(window_name, cv::WINDOW_NORMAL);
        cv::imshow(window_name, downsized_frame);
        cv::waitKey(0);
        cv::destroyAllWindows();
    }
}
