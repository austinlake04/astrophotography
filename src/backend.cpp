#include <libraw/libraw.h>

#include <opencv2/highgui.hpp>
export module backend;

export namespace astrosight {
cv::Mat load(string file) {
  LibRaw raw_processor;

  if (rawProcessor.open_file(file) != LIBRAW_SUCCESS) {
  } else {
    rawProcessor.unpack();

    // rawProcessor.imgdata.params.no_interpolation = 1;
    // rawProcessor.imgdata.params.no_auto_scale = 1;
    // rawProcessor.imgdata.params.no_auto_bright = 1;

    // int check = rawProcessor.dcraw_process();
    // tmpImg = rawProcessor.dcraw_make_mem_image(&check);

    // Init image
    Img = cv::Mat(rawProcessor.imgdata.sizes.width,
                  rawProcessor.imgdata.sizes.height, CV_16UC3,
                  rawProcessor.imgdata.rawdata.raw_image);

    // Init datas
    cameraModel = (std::string)rawProcessor.imgdata.idata.make + "-" +
                  rawProcessor.imgdata.idata.model;
    shutterTime = rawProcessor.imgdata.other.shutter;
  }
}

}  // namespace astrosight
