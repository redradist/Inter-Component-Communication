<a href="https://www.buymeacoffee.com/redradist" target="_blank"><img src="https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png" alt="Buy Me A Coffee" style="height: 41px !important;width: 174px !important;box-shadow: 0px 3px 2px 0px rgba(190, 190, 190, 0.5) !important;-webkit-box-shadow: 0px 3px 2px 0px rgba(190, 190, 190, 0.5) !important;" ></a>

# Inter Component Communication
This is a library created to simplify communication
between components inside of single application.
It is thread safe and could be used for creating
components that works in different threads.

## Integration
To integrate this library in your project you just need in CMake add:
add_subdirectory(<path_to_library>)

## Usage
### Active Object example
Here is simple example of `WeatherStationService` and `DisplayService` that works in different threads:
```c++
#include <icc/Component.hpp>

class WeatherStationService : public icc::Component {
 public:
   icc::Event<void(const double &)> temperature_;

 private:
   void handleTemperature() {
     ...
     temperature_(28.3);
     ...
   }
};

class DisplayService : public icc::Component {
 public:
  explicit DisplayService(WeatherStationService& station) {
    station.temperature_.connect(&DisplayService::onTemperature, this);
  }

  void onTemperature(const double & _temperature) {
     std::cout << "DisplayService::Temperature is " << _temperature << std::endl;
   }
};

int main() {
   auto observer = std::make_shared<WeatherObserver>();
   auto station = std::make_shared<WeatherStation>(*observer);
   auto observerThread = std::thread([&observer] {
     observer->exec();
   });
   auto stationThread = std::thread([&station] {
     station->exec();
   });
   observerThread.join();
   stationThread.join();
}
```

Documentation will be updated in:
[Tutorial](docs/Tutorial.md)

## Known issues
1. Some compilers that supports C++17 at the same time does not support Feature Macros __cpp_lib_optional
If you want to use automatic conversion from icc::_private::containers::Optional<T> to C++17 std::optional<T> you
should set this macros by hand like this:

    add_definitions(-D__cpp_lib_optional=201606) 

## Contributing
1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request :D

## License
MIT License
