#include <libraw/libraw.h>
#include <opencv2/highgui.hpp>
#include <string>
#include <cstddef>

export module backend;

export namespace astrosight {

typedef enum StackMode {
	STANDARD = 0,
	MOSIAC = 1
} StackMode;

std::optional<LibRaw::imgdata> load_frame(string file) {
    LibRaw raw_processor;
    if (raw_processor.open_file(file) != LIBRAW_SUCCESS) {
    } else {
        raw_processor.unpack();
        return raw_processor.imgdata;
    }
    return {};
}

cv::Mat calibrate_frame(cv::Mat light, cv::Mat master_dark, cv::Mat master_flat,
        std::optional<cv::Mat> master_dark_flat = std::nullopt,
        std::optional<cv::Mat> master_bias = std::nullopt) {
    if (master_dark_flat.has_value()) {
        return (light - master_dark) / (master_flat - *master_dark_flat);
    } else if (master_bias.has_value()) {
        return (light - master_dark) / (master_flat - *master_bias);
    }
    return (light - master_dark) / master_flat;
}

cv::Mat raw_to_Mat(LibRaw::imgdata image) {
    return cv::Mat(raw_processor.imgdata.sizes.width,
        raw_processor.imgdata.sizes.height, CV_16UC3,
        raw_processor.imgdata.rawdata.raw_image);
}

std::tuple<cv::Mat, cv::Mat, std::optional<cv::Mat>, std::optional<cv::Mat>>
		create_calibration_frames(vector<string> dark_frames,
			vector<string> flat_frames, vector<string> dark_flat_frames,
			vector<string> bias_frames) {
	cv::Mat master_dark;
	cv::Mat master_flat;
	std::optional<cv::Mat> master_dark_flat;
	std::optional<cv::Mat> master_bias;
	return { master_dark, master_flat, master_dark_flat, master_bias };
}

cv::Mat stack_frames(std::vector<LibRaw::imgdata> image_stack,
		std::tuple<cv::Mat, cv::Mat, std::optional<cv::Mat>, std::optional<cv::Mat> master_calibration_frames,
		StackMode mode) {
	auto [master_dark, master_flat, master_dark_flat, master_bias] = master_calibration_frames; 
    cv::Mat result; 
    for (LibRaw::imgdata image : stack) {
		raw_to_Mat(image);	
    }
    return ;
}


}  // namespace astrosight
