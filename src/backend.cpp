#ifndef BACKEND
#define BACKEND

#include <libraw/libraw.h>
#include <opencv2/core.hpp>
#include <string>
#include <vector>
#include <cstddef>
#include <QQuickImageProvider>
#include <QPixmap>

using std::optional;
using std::nullopt;
using std::tuple;
using std::string;
using std::vector;
using cv::Mat;

namespace astrosight {

class Backend : public QQuickImageProvider {
public:
	Backend() : QQuickImageProvider(QQuickImageProvider::Pixmap) {}
	
	QPixmap requestPixmap(
		const QString &id,
		QSize *size,
		const QSize &requestedSize
	) override {
    	int width = 100;
    	int height = 50;
	
    	if (size) {
			*size = QSize(width, height);
    	}
		
		QPixmap pixmap(requestedSize.width() > 0 ? requestedSize.width() : width,
        	requestedSize.height() > 0 ? requestedSize.height() : height);
    	pixmap.fill(QColor(id).rgba());
    	return pixmap;
    }

	optional<libraw_data_t> load_frame(
		const char * file
	) {
		LibRaw raw_processor;
		if (raw_processor.open_file(file) == LIBRAW_SUCCESS) {
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
		libraw_data_t raw
	) {
		return Mat(raw.sizes.width,
			raw.sizes.height, CV_16UC3,
			raw.rawdata.raw_image);
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
		vector<libraw_data_t> image_stack,
		tuple<Mat, Mat, optional<Mat>, optional<Mat>> master_calibration_frames
	) {
		auto [master_dark, master_flat, master_dark_flat, master_bias] = master_calibration_frames; 
		Mat result; 
		for (libraw_data_t image : image_stack) {
			raw_to_Mat(image);	
		}
		return result;
	}
};

}  // namespace astrosight

#endif
