from conans import ConanFile, CMake, tools


class IccConan(ConanFile):
    name = "icc"
    version = "1.0"
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

    def package_info(self):
        self.cpp_info.name = "icc"
        self.cpp_info.includedirs = ['src']  # Ordered list of include paths
        self.cpp_info.libdirs = ['lib']  # Directories where libraries can be found
        self.cpp_info.bindirs = ['bin']  # Directories where executables and shared libs can be found

    def package(self):
        self.cpp_info.name = "icc"
        self.cpp_info.includedirs = ['src']  # Ordered list of include paths
        self.cpp_info.libdirs = ['lib']  # Directories where libraries can be found
        self.cpp_info.bindirs = ['bin']  # Directories where executables and shared libs can be found

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.test()

    def package(self):
        self.copy("*.h", dst="include", src="icc")
        self.copy("*.hpp", dst="include", src="icc")
        self.copy("*icc.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["icc"]
