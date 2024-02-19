#include <libraw/libraw.h>
#include <opencv2/highgui.hpp>
#include <string>
#include <vector>
#include <cstddef>

using std::optional;
using std::nullopt;
using std::tuple;
using std::vector;
using cv::Mat;


export module backend;

export namespace astrosight {

typedef enum StackMode {
	STANDARD = 0,
	MOSIAC = 1
} StackMode;

optional<LibRaw::imgdata> load_frame(
	string file
) {
    LibRaw raw_processor;
    if (raw_processor.open_file(file) != LIBRAW_SUCCESS) {
    } else {
        raw_processor.unpack();
        return raw_processor.imgdata;
    }
    return {};
}

Mat calibrate_frame(
	Mat light,
	Mat master_dark,
	Mat master_flat,
    optional<Mat> master_dark_flat = nullopt,
    optional<Mat> master_bias = nullopt
) {
    if (master_dark_flat.has_value()) {
        return (light - master_dark) / (master_flat - *master_dark_flat);
    } else if (master_bias.has_value()) {
        return (light - master_dark) / (master_flat - *master_bias);
    }
    return (light - master_dark) / master_flat;
}

Mat raw_to_Mat(
	LibRaw::imgdata image
) {
    return Mat(raw_processor.imgdata.sizes.width,
        raw_processor.imgdata.sizes.height, CV_16UC3,
        raw_processor.imgdata.rawdata.raw_image);
}

tuple<Mat, Mat, optional<Mat>, optional<Mat>> create_calibration_frames(
	vector<string> dark_frames,
	vector<string> flat_frames,
	vector<string> dark_flat_frames,
	vector<string> bias_frames
) {
	Mat master_dark;
	Mat master_flat;
	optional<Mat> master_dark_flat;
	optional<Mat> master_bias;
	return { master_dark, master_flat, master_dark_flat, master_bias };
}

Mat stack_frames(
	vector<LibRaw::imgdata> image_stack,
	tuple<Mat, Mat, optional<Mat>, optional<Mat> master_calibration_frames,
	StackMode mode
) {
	auto [master_dark, master_flat, master_dark_flat, master_bias] = master_calibration_frames; 
    Mat result; 
    for (LibRaw::imgdata image : stack) {
		raw_to_Mat(image);	
    }
    return ;
}


}  // namespace astrosight
