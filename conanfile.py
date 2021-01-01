from conans import ConanFile, CMake, tools


class IccConan(ConanFile):
    name = "icc"
    version = "1.0.0"
    license = "MIT"
    author = "Denis Kotov, redradist@gmail.com"
    url = "https://github.com/redradist/Inter-Component-Communication.git"
    description = "I.C.C. - Inter Component Communication, This is a library created to simplify communication between " \
                  "components inside of single application. It is thread safe and could be used for creating " \
                  "components that works in different threads. "
    topics = ("thread-safe", "active object")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    generators = "cmake"

    def source(self):
        self.run("git clone https://github.com/redradist/Inter-Component-Communication.git .")
        self.run("git submodule update --init --recursive")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        # TODO(redradist): Fix running tests during packaging
        # cmake.test()

    def package(self):
        self.copy("*.h", src="src", dst="include")
        self.copy("*.hpp", src="src", dst="include")
        self.copy("*ICC.lib", dst="lib", keep_path=False)
        self.copy("*ICC.dll", dst="bin", keep_path=False)
        self.copy("*ICC.so", dst="lib", keep_path=False)
        self.copy("*ICC.dylib", dst="lib", keep_path=False)
        self.copy("*ICC.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.name = "icc"
        self.cpp_info.libs = ["ICC"]
        self.cpp_info.includedirs = ['include']
        self.cpp_info.libdirs = ['lib']
        self.cpp_info.bindirs = ['bin']
