# Inter Component Communication
This is a library created to simplify communication
between components inside of single application.
It is thread safe and could be used for creating
components that works in different threads.

## Integration
To integrate this library in your project you just need in CMake add:
add_subdirectory(<path_to_library>)

## Usage
For introduction to ICC library see:
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

## History
TODO: Write history

## Credits
TODO: Write credits

## License
MIT License
