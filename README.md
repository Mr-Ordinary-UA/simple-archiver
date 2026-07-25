# simple-archiver

A file archiver based on the RLE (Run-Length Encoding) algorithm. This project demonstrates file handling, binary data processing, and C++ performance with Python GUI integration.

## RLE Description

RLE replaces identical repeating byte sequences with count-value pairs. A sequence of ten identical characters takes less space in compressed form than in the original file.

The method works well on files with repeating data. On random or previously compressed data, the size after compression does not decrease. The application detects such cases and saves raw unmodified data.

## Version 2.x (GUI Version)

Graphical interface version built with Python Tkinter and C++ core via pybind11.

### Features

- Graphical interface built with Tkinter
- Fast C++ compression core integrated via pybind11
- Automated cross-platform builds for Windows, Linux, and macOS

### Build Requirements

- CMake 3.15 or newer
- C++17 compiler (MSVC, GCC, or Clang)
- Python 3.11 or newer
- PyInstaller module

### Building Version 2.x

Clone the repository:

```bash
git clone [https://github.com/Mr-Ordinary-UA/simple-archiver.git](https://github.com/Mr-Ordinary-UA/simple-archiver.git)
cd simple-archiver
```

Build the C++ core:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Package the application:

```bash
cd python
pip install pyinstaller
python -m PyInstaller --noconsole --onefile --add-data "archive_py.pyd;." gui.py
```

Executable output paths:

- Windows: python/dist/gui.exe
- Linux: python/dist/gui
- macOS: python/dist/gui

### How to Use Version 2.x

Run the executable file.

Compressing a file:

1. Click the Compress File button.
2. Select the file in the dialog window.
3. Select the destination path.

Decompressing an archive:

1. Click the Decompress File button.
2. Select the archive file.
3. Select the destination path.

---

## Version 1.x (Console Version)

Console application written in C++17.

### Features

- Text menu interface
- Direct C++ file processing
- Interactive terminal menu

### Build Requirements

- CMake 3.15 or newer
- C++17 compiler (MSVC, GCC, or Clang)

### Building Version 1.x

Clone the repository:

```bash
git clone [https://github.com/Mr-Ordinary-UA/simple-archiver.git](https://github.com/Mr-Ordinary-UA/simple-archiver.git)
cd simple-archiver
```

Build the project:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Executable output paths:

- Windows (MSVC): build/Debug/archive.exe
- Windows (MinGW): build/archive.exe
- Linux: build/archive

### How to Use Version 1.x

Run the terminal application. Select an option from the menu:

1. Compress file
2. Decompress file
3. Exit

Compressing a file:

1. Select option 1.
2. Enter the source file path.
3. Enter the destination archive path.

Decompressing an archive:

1. Select option 2.
2. Enter the archive path.
3. Enter the destination file path.

---

## Pre-built Executables

Pre-compiled binaries for Windows, Linux, and macOS are available in the Releases section of this repository.

## Archive Format Structure

The binary file begins with the RLE1 signature header. The next byte indicates the compression mode. The application verifies this header before extraction.

## License

This project is released under the MIT license. The full text is available in the LICENSE file.
