import numpy as np

from astrosight.core import downsample


def test_downsample():
    image = np.random.randint(
        low=0, high=np.iinfo(np.uint16).max, size=(640, 480), dtype=np.uint16
    )
    image = downsample(image)
    assert (
        image.dtype == np.uint8
    ), "Image bit depth was not downsampled to 8 bits per pixel."
