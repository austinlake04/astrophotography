"""Core image processing functionality."""

import argparse
import os
import sys
from tkinter import filedialog as fd
from typing import Optional

import cv2
import matplotlib.pyplot as plt
import numpy as np
import rawpy
from astropy.io import fits
from PySide6.QtGui import QGuiApplication
from PySide6.QtWidgets import QFileDialog
from scipy.signal import find_peaks
from tqdm import tqdm

class Backend:
    def __init__(self):
        self.images = {
            "Light" = [],
            "Bias" = [],
            "Dark" = [],
            "Flat" = [],
            "Dark Falt" = []
        }
        self.params = None

    def set_params(
        demosaic_algorithm=None,
        half_size=False,
        four_color_rgb=False,
        dcb_iterations=0,
        dcb_enhance=False,
        fbdd_noise_reduction=FBDDNoiseReductionMode.Off,
        noise_thr=None,
        median_filter_passes=0,
        use_camera_wb=False, use_auto_wb=False,
        user_wb=None,
        output_color=ColorSpace.sRGB,
        output_bps=8,
        user_flip=None,
        user_black=None,
        user_sat=None,
        no_auto_bright=False,
        auto_bright_thr=None,
        adjust_maximum_thr=0.75,
        bright=1.0,
        highlight_mode=HighlightMode.Clip,
        exp_shift=None,
        exp_preserve_highlights=0.0,
        no_auto_scale=False,
        gamma=None,
        chromatic_aberration=None,
        bad_pixels_path=None
    ) -> None:
        self.postprocess_params = rawpy.Params(
            demosaic_algorithm=None,
            half_size=False,
            four_color_rgb=False,
            dcb_iterations=0,
            dcb_enhance=False,
            fbdd_noise_reduction=FBDDNoiseReductionMode.Off,
            noise_thr=None,
            median_filter_passes=0,
            use_camera_wb=False, use_auto_wb=False,
            user_wb=None,
            output_color=ColorSpace.sRGB,
            output_bps=8,
            user_flip=None,
            user_black=None,
            user_sat=None,
            no_auto_bright=False,
            auto_bright_thr=None,
            adjust_maximum_thr=0.75,
            bright=1.0,
            highlight_mode=HighlightMode.Clip,
            exp_shift=None,
            exp_preserve_highlights=0.0,
            no_auto_scale=False,
            gamma=None,
            chromatic_aberration=None,
            bad_pixels_path=None
        )

    def preview(
        self,
        raw_file: str
    ) -> np.ndarray:
        """Loads preview of RAW image.

        Parameters
        ----------
        raw_file : str
            Path to RAW file

        Returns
        -------
        raw_image : numpy.ndarray
            Data for RAW image
        """
        assert os.path.isfile(raw_file), "RAW file doesn't exist."
        with rawpy.imread(raw_file) as raw:
            raw_image = raw.raw_image
        return raw_image


    def master_calibration(
        self,
        file_tree: dict[list[str]],
        quiet: bool = False
    ) -> tuple[Optional[np.ndarray], Optional[np.ndarray]]:
        """Generates calibration frame from a specified image set.

        Paramters
        ---------
        file_tree : dict[list[str]]
            File tree containing light, dark, bias, and flat images.
        quiet : bool
            Hides text printed to the terminal

        Returns
        -------
        master_dark : Optional[numpy.ndarray]
            Master dark calibration frame
        master_flat : Optional[numpy.ndarray]
            Master flat calibration frame
        """
        try:
            master_bias = None
            if len(file_tree["Bias"]):
                for i, bias_file in enumerate(
                    tqdm(file_tree["Bias"], desc="Loading bias frames", disable=quiet)
                ):
                    if bias_file[-5::] == ".fits" or bias_file[-4::] == ".fts":
                        with fits.open(bias_file) as hdul:
                            if master_bias is None:
                                master_bias = np.empty(
                                    (len(file_tree["Bias"]), *hdul[0].data.shape)
                                )
                            master_bias[i] = hdul[0].data
                    else:
                        with rawpy.imread(bias_file) as raw:
                            if master_bias is None:
                                master_bias = np.empty(
                                    (len(file_tree["Bias"]), *raw.raw_image.shape)
                                )
                            master_bias[i] = raw.raw_image
            if master_bias is not None:
                master_bias = np.mean(master_bias, axis=0)
        except rawpy.LibRawError:
            print("Error occured. No master bias frame created.")
            master_bias = None

        try:
            master_dark = None
            if len(file_tree["Dark"]):
                for i, dark_file in enumerate(
                    tqdm(file_tree["Dark"], desc="Loading dark frames", disable=quiet)
                ):
                    if dark_file[-5::] == ".fits" or dark_file[-4::] == ".fts":
                        with fits.open(dark_file) as hdul:
                            if master_dark is None:
                                master_dark = np.empty(
                                    (len(file_tree["Dark"]), *hdul[0].data.shape)
                                )
                            master_dark[i] = hdul[0].data
                    else:
                        with rawpy.imread(dark_file) as raw:
                            if master_dark is None:
                                master_dark = np.empty(
                                    (len(file_tree["Dark"]), *raw.raw_image.shape)
                                )
                            master_dark[i] = raw.raw_image
            if master_dark is not None:
                master_dark = np.median(master_dark, axis=0)
        except rawpy.LibRawError:
            print("Error occured. No master dark frame created.")
            master_dark = None

        try:
            master_dark_flat = None
            if len(file_tree["Dark Flat"]):
                for i, dark_flat_file in enumerate(
                    tqdm(
                        file_tree["Dark Flat"],
                        desc="Loading dark flat frames",
                        disable=quiet,
                    )
                ):
                    if dark_flat_file[-5::] == ".fits" or dark_flat_file[-4::] == ".fts":
                        with fits.open(dark_flat_file) as hdul:
                            if master_dark_flat is None:
                                master_dark_flat = np.empty(
                                    (len(file_tree["Dark Flat"]), *hdul[0].data.shape)
                                )
                            master_dark_flat[i] = hdul[0].data
                    else:
                        with rawpy.imread(dark_flat_file) as raw:
                            if master_dark_flat is None:
                                master_dark_flat = np.empty(
                                    (len(file_tree["Dark Flat"]), *raw.raw_image.shape)
                                )
                            master_dark_flat[i] = raw.raw_image
            if master_dark_flat is not None:
                master_dark_flat = np.median(master_dark_flat, axis=0)
                if master_bias:
                    master_dark_flat -= master_bias
        except rawpy.LibRawError:
            print("Error occured. No master dark flat frame created.")
            master_dark_flat = None

        try:
            master_flat = None
            if len(file_tree["Flat"]):
                for i, flat_file in enumerate(
                    tqdm(
                        file_tree["Flat"],
                        desc="Loading flat frames",
                        disable=quiet
                    )
                ):
                    if flat_file[-5::] == ".fits" or flat_file[-4::] == ".fts":
                        with fits.open(flat_file) as hdul:
                            if master_flat is None:
                                master_flat = np.empty(
                                    (len(file_tree["Flat"]), *hdul[0].data.shape)
                                )
                            master_flat[i] = hdul[0].data
                    else:
                        with rawpy.imread(flat_file) as raw:
                            if master_flat is None:
                                master_flat = np.empty(
                                    (len(file_tree["Flat"]), *raw.raw_image.shape)
                                )
                            master_flat[i] = raw.raw_image
            if master_flat is not None:
                master_flat = np.median(master_flat, axis=0)
                if master_bias:
                    master_flat -= master_bias
                if master_dark_flat:
                    master_flat -= master_dark_flat
        except rawpy.LibRawError:
            print("Error occured. No master flat frame created.")
            master_flat = None

        if not quiet:
            print("Master calibration frames created!")

        return master_dark, master_flat


    def load_image(
        self
        raw_file: str,
        master_dark: Optional[np.ndarray] = None,
        master_flat: Optional[np.ndarray] = None,
        feature_detector: str = "ORB",
    ) -> tuple[Optional[np.ndarray], Optional[tuple[cv2.KeyPoint]], Optional[np.ndarray]]:
        """Loads and processes an astrophotography image at a specified file path.

        Paramters
        ---------
        raw_file : str
            Path to specified RAW file.
        master_dark : numpy.ndarray
            Master dark calibration frame.
        master_flat : numpy.ndarray
            Master flat calibration frame.
        feature_detector : str
            Feature detector (ORB, SIFT, or AKAZE) to indentify prominent stars.

        Returns
        -------
        image : Optional[numpy.ndarray]
            Calibrated and processed image
        keypoints : Optional[tuple[cv2.KeyPoint]]
            Keypoints from features of interest for calibrated image
        descriptors : Optional[np.ndarray]
            Descriptors from features of interest for calibrated image
        """
        assert feature_detector in [
            "ORB",
            "SIFT",
            "AKAZE",
        ], "Invalid feature detector type."

        try:
            if raw_file[-5::] == ".fits" or raw_file[-4::] == ".fts":
                with fits.open(raw_file) as hdul:
                    image = hdul[0].data
                    image = calibrate(image, master_dark, master_flat)
            else:
                with rawpy.imread(raw_file) as raw:
                    image = raw.raw_image
                    image = calibrate(image, master_dark, master_flat)
                    np.copyto(raw.raw_image, image)
                    image = raw.postprocess(self.postprocess_params)
            if np.max(image) < 255:
                image = 257 * image
            peaks, _ = find_peaks(image.flatten(), height=7 * 257)
            row, col = divmod(peaks, image.shape[0])
            save = image[row, col]
            image = image.astype(np.uint16)
            image[row, col] = np.iinfo(np.uint16).max
            if feature_detector == "ORB":
                detector = cv2.ORB_create()
            elif feature_detector == "SIFT":
                detector = cv2.SIFT_create()
            else:
                detector = cv2.AKAZE_create()
            keypoints, descriptors = detector.detectAndCompute(downsample(image), None)
            image[row, col] = save
        except rawpy.LibRawError:
            print("Error occured. Image not loaded.")
        else:
            return image, keypoints, descriptors
        return None, None, None


    def calibrate_image(
        image: np.ndarray,
        master_dark: Optional[np.ndarray] = None,
        master_flat: Optional[np.ndarray] = None,
    ) -> np.ndarray:
        """Calibrates an image.

        Parameters
        ----------
        image : numpy.ndarray
            Uncalibrated image.
        master_dark : numpy.ndarray
            Master dark calibration frame.
        master_flat : numpy.ndarray
            Master flat calibration frame.

        Returns
        -------
        image : numpy.ndarray
            Calibrated image
        """
        image = 1.0 * image  # floating point conversion for data retention in computation
        if master_dark is not None:
            image = image - master_dark
        if master_flat is not None:
            image = image / master_flat
        image[image < 0] = 0
        image = image.astype(np.uint16)
        return image


    def display_image(
        image: np.ndarray,
        filename: Optional[str] = None,
        save: bool = False
    ) -> None:
        """Displays an image.

        Paramters
        ---------
        image : numpy.ndarray
            Image to be displayed.
        filename : Optional[str]
            Filename in which to save resulting image.
        save : bool
            Determines if resulting image is automatically saved.

        Returns
        -------
        None
        """
        fig, ax = plt.subplots()
        ax.imshow(downsample(image), cmap=(None if len(image.shape) == 3 else "gray"))
        plt.style.use("dark_background")
        plt.title((filename.replace("_", " ") if filename else ""))
        plt.axis("off")
        if save:
            plt.savefig(f"{filename.replace(' ', '_')}", bbox_inches="tight", pad_inches=0)
        plt.show()
        plt.close(fig)


    def register_image(
        image: np.ndarray,
        features: tuple[tuple[cv2.KeyPoint], np.ndarray],
        base_features: tuple[tuple[cv2.KeyPoint], np.ndarray],
        feature_detector: str = "ORB",
        match_threshhold: float = 0.8,
    ) -> np.ndarray:
        """Registers and aligns an image

        Parameters
        ----------
        image : numpy.ndarray
            Calibrated image
        features : tuple[tuple[cv2.KeyPoint], numpy.ndarray]
            Keypoints and descriptors from features of interest for current image.
        base_features : tuple[tuple[cv2.KeyPoint], numpy.ndarray]
            Keypoints and descriptors for features of interest of reference image.
        feature_detector : str
            Feature detector (ORB, SIFT, or AKAZE) to indentify prominent stars.
        match_threshhold : float
            Percentage of matches to include in registration process.

        Returns
        -------
        image : numpy.ndarray
            Calibrated and registered image.
        """
        assert feature_detector in ["ORB", "SIFT", "AKAZE"], "Invalid feature detector."

        keypoints, descriptors = features
        base_keypoints, base_descriptors = base_features

        matcher = (
            cv2.BFMatcher(cv2.NORM_HAMMING, crossCheck=True)
            if feature_detector != "SIFT"
            else cv2.BFMatcher(cv2.NORM_L2, crossCheck=True)
        )
        matches = matcher.match(descriptors, base_descriptors, None)
        matches = sorted(matches, key=lambda x: x.distance)

        if matcher == "SIFT":
            good_matches = []
            for m1, m2 in matches:
                if m1.distance < 0.6 * m2.distance:
                    good_matches.append(m1)
            matches = good_matches
        else:
            matches = matches[: int(len(matches) * match_threshhold)]

        features = [keypoints, base_keypoints]
        points = np.empty((2, len(matches), 2))
        for i, feature in enumerate(features):
            for j, match in enumerate(matches):
                points[i, j, :] = feature[match.trainIdx if i else match.queryIdx].pt
        try:
            homography, mask = cv2.findHomography(points[0], points[1], cv2.RANSAC)
            image = cv2.warpPerspective(image, homography, (image.shape[1], image.shape[0]))
        except cv2.error:
            dx = int(base_keypoints[0].pt[0] - keypoints[0].pt[0])
            dy = int(base_keypoints[0].pt[1] - keypoints[0].pt[1])
            image = np.roll(image, dx, axis=1)
            image = np.roll(image, dy, axis=0)
        return image


    def stack_images(
        file_tree: dict[list[str]],
        feature_detector: str = "ORB",
        filename: Optional[str] = None,
        save: bool = False,
        quiet: bool = False,
    ) -> np.ndarray:
        """Stacks astrophotography images.

        Parameters
        ----------
        file_tree : dict[list[str]]
            Absolute paths to light, bias, dark, dark flat, and flat images.
        feature_detector : str
            Feature detector (ORB, SIFT, or AKAZE) to indentify prominent stars.
        filename : Optional[str]
            Filename in which to save resulting image.
        save : bool
            Determines if resulting image is automatically saved.
        verbose : bool
            Hides text printed to the terminal.

        Returns
        -------
        stacked_image: numpy.ndarray
            Stacked image
        """
        try:
            image_stack = None
            master_dark, master_flat = master_calibration(file_tree, quiet=quiet)
            for i, raw_file in enumerate(
                tqdm(file_tree["Light"], desc="Loading images", disable=quiet)
            ):
                image, keypoints, descriptors = load(
                    raw_file=raw_file,
                    master_dark=master_dark,
                    master_flat=master_flat,
                    feature_detector=feature_detector,
                )
                if image is not None:
                    if image_stack is None:
                        image_stack = np.empty((len(file_tree["Light"]), *image.shape))
                        image_stack[0], base_keypoints, base_descriptors = (
                            image,
                            keypoints,
                            descriptors,
                        )
                    else:
                        registered_image = register(
                            image,
                            features=(keypoints, descriptors),
                            base_features=(base_keypoints, base_descriptors),
                            feature_detector=feature_detector,
                        )
                        image_stack[i] = registered_image
            if image_stack is not None:
                stacked_image = np.sum(
                    image_stack, axis=0
                )  # sigmaclip(np.sum(image_stack, axis=0), 5, 5)[0]
                display(stacked_image, filename=filename, save=save)
                print(image_stack.shape)
                return stacked_image
        except rawpy.LibRawError:
            print("Failed to created stacked image.")
        else:
            pass
        return None
