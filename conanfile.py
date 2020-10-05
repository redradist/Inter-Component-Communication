from conans import ConanFile, CMake, tools


class HelloConan(ConanFile):
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

    def configure(self):
        del self.settings.compiler.libcxx

    def source(self):
        self.run("git clone https://github.com/redradist/Inter-Component-Communication.git")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        # here you can run CTest, launch your binaries, etc
        cmake.test()