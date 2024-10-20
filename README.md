# aegyo

[![CI](https://github.com/ryouze/aegyo/actions/workflows/ci.yml/badge.svg)](https://github.com/ryouze/aegyo/actions/workflows/ci.yml)
[![Release](https://github.com/ryouze/aegyo/actions/workflows/release.yml/badge.svg)](https://github.com/ryouze/aegyo/actions/workflows/release.yml)
![Release version](https://img.shields.io/github/v/release/ryouze/aegyo)

aegyo is a cross-platform GUI app for learning Korean Hangul.


## Motivation

When I was learning Japanese kana, I liked the idea of drilling the characters into my memory through pure brute-force repetition. I found an iOS app called [Kana School](https://apps.apple.com/us/app/kana-school-japanese-letters/id1214626499) that did exactly that. Unfortunately, no similar app existed for Korean Hangul, so I decided to create one myself.


## Features

- Written in modern C++ (C++17).
- Comprehensive documentation with doxygen-style comments.
- Automatic third-party dependency management using CMake's [FetchContent](https://www.foonathan.net/2022/06/cmake-fetchcontent/).
- No missing STL headers thanks to [header-warden](https://github.com/ryouze/header-warden).


## Tested Systems

This project has been tested on the following systems:

- macOS 15.0.1 (Sequoia)
- Manjaro 24.0 (Wynsdey)
- Windows 11 23H2

Automated testing is also performed on the latest versions of macOS, GNU/Linux, and Windows using GitHub Actions.


## Pre-built Binaries

Pre-built binaries are available for macOS (ARM64), GNU/Linux (x86_64), and Windows (x86_64). You can download the latest version from the [Releases](../../releases) page.

To remove macOS quarantine, use the following command:

```sh
xattr -d com.apple.quarantine aegyo-macos-arm64.app
chmod +x aegyo-macos-arm64.app
```

On Windows, the app might be detected as a virus by Windows Defender, so you'll have to add it to the [exclusions list](https://support.microsoft.com/en-us/windows/add-an-exclusion-to-windows-security-811816c0-4dfd-af4a-47e4-c301afe13b26). You can upload the binary to [VirusTotal](https://www.virustotal.com/gui/home/upload) and read the source code to verify that it's safe. As of right now, the app is detected as a false positive by 2 out of 73 antivirus engines, which includes the built-in Windows Defender. Other SFML apps I have built in the past did not have this issue, so I'm not sure what's causing it.


## Requirements

To build and run this project, you'll need:

- C++17 or higher
- CMake


## Build

Follow these steps to build the project:

1. **Clone the repository**:

    ```sh
    git clone https://github.com/ryouze/aegyo.git
    ```

2. **Generate the build system**:

    ```sh
    cd aegyo
    mkdir build && cd build
    cmake ..
    ```

    Optionally, you can disable compile warnings by setting `ENABLE_COMPILE_FLAGS` to `OFF`:

    ```sh
    cmake .. -DENABLE_COMPILE_FLAGS=OFF
    ```

3. **Compile the project**:

    To compile the project, use the following command:

    ```sh
    make
    ```

    If you want to use all available cores when compiling with `make`, you can pass the `-j` flag along with the number of cores available on your system:

    - **macOS**:

      ```sh
      make -j$(sysctl -n hw.ncpu)
      ```

    - **GNU/Linux**:

      ```sh
      make -j$(nproc)
      ```

    - **8-core CPU**:

      ```sh
      make -j8
      ```

After successful compilation, you can run the program using `./aegyo` (`open aegyo.app` on macOS). However, it is highly recommended to install the program, so that it can be run from any directory. Refer to the [Install](#install) section below.

**Note:** The mode is set to `Release` by default. To build in `Debug` mode, use `cmake .. -DCMAKE_BUILD_TYPE=Debug`.


## Install

If not already built, follow the steps in the [Build](#build) section and ensure that you are in the `build` directory.

To install the program, use the following command:

```sh
sudo cmake --install .
```

On macOS, this will install the program to `/Applications`. You can then run `aegyo` from the Launchpad, Spotlight, or by double-clicking the app in Finder.


## Usage

To run the program, use the following command:

```sh
aegyo
```


On first run on macOS, the app will request access to keystrokes from any application:

```
"aegyo.app" would like to receive keystrokes from any application.

Grant access to this application in Privacy & Security settings, located in System Settings.
```

This is caused by the underlying SFML library, which reads raw keyboard input. You should **deny** this request, as the app does not expect any input while it's not in focus. I'd rather not have such a request appear in the first place, but I have no control over it.

### Controls

You can select the correct answer by clicking on it or by pressing the corresponding number key on your keyboard (1, 2, 3, 4).

The buttons in the top right corner toggle the vocabulary category:

- `Vow` for Basic Vowels
- `Con` for Basic Consonants
- `DCon` for Double Consonants
- `CompV` for Compound Vowels

All categories are enabled by default. You can disable a category by clicking on the corresponding button.

If you're a beginner, start with the `Vow` categories and gradually enable the other categories as you continue to learn.


## Testing

Tests are included in the project but are not built by default.

To enable and build the tests manually, run the following commands:

```sh
cmake .. -DBUILD_TESTS=ON
make
ctest --output-on-failure
```


## Credits

- [fmt](https://github.com/fmtlib/fmt)
- [Language](https://macosicons.com/#/u/Bonjour)
- [Let's Learn Hangul!](http://letslearnhangul.com/)
- [Nanum Gothic](https://fonts.google.com/specimen/Nanum+Gothic)
- [Simple and Fast Multimedia Library](https://github.com/sfml/sfml)


## Contributing

All contributions are welcome.


## License

This project is licensed under the MIT License.
