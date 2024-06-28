from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout

required_conan_version = ">=2.0"

class Recipe(ConanFile):
    name = "astrosight"
    version = "0.1.0"
    license = "Apache-2.0"
    author = "Austin Lucas Lake (53884490+austinlucaslake@users.noreply.github.com)"
    url = "https://github.com/austinlucaslake/astrosight"
    description = "Image processing application for astrophotography."
    topics = ("astrophotography", "image processing")
    settings = "arch", "compiler", "build_type", "os"
    exports_sources = "CMakeLists.txt", "src/*", "include/*"
   
    def requirements(self):
        self.requires("argparse/3.0")
        self.requires("ccfits/2.6")
        self.requires("libraw/0.21.1")
        self.requires("opencv/4.8.1")
        self.requires("qt/6.7.1")
        self.requires("jasper/4.2.0", override=True)
        self.requires("xkbcommon/1.6.0", override=True)
        self.requires("ffmpeg/6.1", override=True)
        self.requires("libpng/1.6.43", override=True)

    def build_requirements(self):
        self.tool_requires("cmake/[>3.28]")
        self.test_requires("cppcheck/2.12")
        self.test_requires("uncrustify/0.78.0")

    def generate(self):
        toolchain = CMakeToolchain(self)
        toolchain.generate()
        dependancies = CMakeDeps(self)
        dependancies.check_components_exist = True
        dependancies.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        # cmake.test()

    def layout(self):
        cmake_layout(self, src_folder="src", build_folder="build")
