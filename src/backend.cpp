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

QImage astrosight::Backend::create_qimage(const image_t& image, const bool thumbnail) {
    QImage::Format format;
    switch (image.frame.type()) {
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
        (uchar*) image.frame.data,
        image.frame.cols,
        image.frame.rows,
        image.frame.step,
        QImage::Format_Grayscale16
    ).rgbSwapped();
}
*/

void astrosight::Backend::load_frames(vector<string>& files, image_type_t type) {
    vector<image_t>& images = light_images;
    switch (type) {
        case image_type_t::dark:
            images = dark_images;
            break;       
        case image_type_t::flat:
            images = flat_images;
            break;       
        case image_type_t::dark_flat:
            images = dark_flat_images;
            break;       
        case image_type_t::bias:
            images = bias_images;
            break;
        case image_type_t::blue:
            images = blue_images;
            break;
        case image_type_t::green:
            images = green_images;
            break;
        case image_type_t::red:
            images = red_images;
            break;
        default:
            break;
    }
    for (string file : files) {
        if (std::find_if(images.begin(), images.end(), [&file](const image_t& image) { return image.file == file; }) == images.end()) {
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
                images.push_back((image_t) {
                    move(frame),
                    move(file),
                    false,
                    std::nullopt
                });
                /*
                    move(type),
                    system_clock::to_time_t(system_clock::now()),
                    "",
                    "", 
                    0,
                    0,
                    0,
                    0
                */
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
                    images.push_back((astrosight::image_t) {
                        move(frame(cv::Rect(
                            raw.imgdata.sizes.left_margin,
                            raw.imgdata.sizes.top_margin,
                            raw.imgdata.sizes.width,
                            raw.imgdata.sizes.height
                        ))),
                        move(file),
                        false,
                        std::nullopt
                    });
                    /*
                        move(type),
                        move(raw.imgdata.other.timestamp),
                        move(raw.imgdata.idata.make),
                        move(raw.imgdata.idata.model),
                        move(raw.imgdata.other.iso_speed),
                        move(raw.imgdata.other.shutter),
                        move(raw.imgdata.other.aperture),
                        move(raw.imgdata.other.focal_len)
                    */
                }
                raw.recycle();
            }
        }
    }
}

void astrosight::Backend::calibrate_frames() {
    auto calibration = [this](vector<image_t>& images){
        for (image_t& image : images) {
            if (!image.calibrated) {
                image.frame.convertTo(image.frame, CV_32F);
                //cv::normalize(image.frame, image.frame, 0, 1, cv::NORM_MINMAX);
                if (this->master_dark_image) {
                    image.frame -= (*this->master_dark_image).frame;
                }
                if (this->master_flat_image && this->master_dark_flat_image) {
                    image.frame /= (*this->master_flat_image).frame - (*this->master_dark_flat_image).frame;
                } else if (this->master_flat_image && this->master_bias_image) {
                    image.frame /= (*this->master_flat_image).frame - (*this->master_bias_image).frame;
                } else if (this->master_flat_image) {
                    image.frame /= (*this->master_flat_image).frame;
                }

                image.calibrated = true;
            }
        }
    };

    switch (color_mode) {
        case color_mode_t::colored:
            calibration(light_images);
            break;
        case color_mode_t::monochrome:
            calibration(red_images);
            calibration(green_images);
            calibration(blue_images);
            break;
        default:
            break;
    }

}

void astrosight::Backend::create_rgb_frames() {
    switch (color_mode) {
        case color_mode_t::colored:
            for (image_t& light_image : light_images) {
                if (light_image.frame.channels() == 1) {
                    light_image.frame.convertTo(light_image.frame, CV_16U);
                    cv::demosaicing(
                        light_image.frame,
                        light_image.frame,
                        demosaic_algorithm
                    );
                    light_image.frame.convertTo(light_image.frame, CV_32F);
                }
            };
            break;
        /*
        case color_mode_t::monochrome:
            if (blue_images.size() == green_images.size() == red_images.size()) {
                light_images.clear();
                for (auto& [blue_image, green_image, red_image] :
		    std::ranges::zip_view(blue_images, green_images, red_images)
                ) {
                    Mat.frame = cv::merge(
                        blue_image.frame,
                        green_image.frame,
                        red_image.frame
                    );
                    light_images.emplace_back(
                       .frame,
                        "", 
                        image_type_t::light,
                        system_clock::to_time_t(system_clock::now()),
                        blue_image.make,
                        blue_image.model,
                        blue_image.iso,
                        blue_image.shutter_speed,
                        blue_image.aperture,
                        blue_image.focal_length,
                        blue_image.stacked,
                        blue_image.calibrated,
                        blue_image.keypoints
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
    if (!reference_image) {
        if (!quiet) { cout << "No reference frame selected. Selecting for you." << endl; }
        switch (color_mode) {
            case color_mode_t::colored:
                if (light_images.size()) { reference_image = make_shared<image_t>(light_images[0]); }
                else {
                    cerr << "No frames loaded. Unable to auto-select a reference frame." << endl;
                    return;
                }
                break;
            case color_mode_t::monochrome:
                if (blue_images.size()) { reference_image = make_shared<image_t>(blue_images[0]); }
                else if (green_images.size()) { reference_image = make_shared<image_t>(green_images[0]); }
                else if (red_images.size()) { reference_image = make_shared<image_t>(red_images[0]); }
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
        reference_image->frame,
        cv::noArray(),
        reference_keypoints,
        reference_descriptors
    );
    reference_image->keypoints = reference_keypoints;

    for (image_t& light_image : light_images) {
        vector<KeyPoint> keypoints;
        if (reference_image.get() == &light_image) {    
            Mat descriptors;
            this->detector->detectAndCompute(
                light_image.frame,
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
                    light_image.frame,
                    light_image.frame,
                    homography,
                    light_image.frame.size()
                );
                // cv::perspectiveTransform(keypoints, keypoints, homography);
            } catch (const std::exception& err) {
                cerr << err.what() << endl;
            }
        }
        light_image.keypoints = keypoints;
    }
}

void astrosight::Backend::create_master_frames() {
    auto average = [](
        const vector<image_t>& images
    ) {
        optional<image_t> master_image;
        if (images.size()) {
            Mat master_frame(images[0].frame.size(), CV_32FC1);
            for (const image_t& image: images) { master_frame += image.frame; }
            master_frame /= images.size();
            master_image = (image_t) {
                .frame = move(master_frame), 
                .file = "",
                .calibrated = false,
                .keypoints = std::nullopt
            };
            /*
                .type = images[0].type,
                .time = system_clock::to_time_t(system_clock::now()),
                .make = images[0].make,
                .model = images[0].model,
                .iso = images[0].iso,
                .shutter_speed = images[0].shutter_speed,
                .aperture = images[0].aperture,
                .focal_length = images[0].focal_length,
                .stacked = true,
            */
        }
        return master_image;
    };

    master_dark_image = average(dark_images);
    master_flat_image = average(flat_images);
    master_dark_flat_image = average(dark_flat_images);
    master_bias_image = average(bias_images);
}

void astrosight::Backend::stack_frames() {
    optional<image_t> master_image;
    if (light_images.size()) {
        Mat master_frame(light_images[0].frame.size(), CV_32FC3);
        for (const image_t& image: light_images) { master_frame += image.frame; }
        master_image = (image_t) {
            .frame = move(master_frame),
            .file = "",
            .calibrated = light_images[0].calibrated,
            .keypoints = light_images[0].keypoints
        };
        /*
            .file = "", 
            .type = image_type_t::light,
            .time = system_clock::to_time_t(system_clock::now()),
            .make = light_images[0].make,
            .model = light_images[0].model,
            .iso = light_images[0].iso,
            .shutter_speed = light_images[0].shutter_speed,
            .aperture = light_images[0].aperture,
            .focal_length = light_images[0].focal_length,
            .stacked = true,
        */
    }
    stacked_image = master_image;
}

void astrosight::Backend::set_preview(image_t& image) { preview_image = std::make_shared<image_t>(image); }

void astrosight::display_frame(const image_t& image) {
    if (!(image.frame.empty())) {
        Mat preview;
        cv::resize(
            image.frame,
            preview,
            cv::Size(),
            0.25,
            0.25,
            cv::INTER_AREA
        );
        cv::imshow("preview", preview);
        cv::waitKey(0);
        cv::destroyAllWindows();
    }
}
