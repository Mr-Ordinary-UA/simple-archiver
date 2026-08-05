# Simple Archiver

A file archiver based on the Run-Length Encoding algorithm. This project demonstrates file handling, binary data processing, and C++ performance with Python GUI integration.

## RLE Description

Run-Length Encoding replaces identical repeating byte sequences with count-value pairs. A sequence of ten identical characters takes less space in compressed form than in the original file.

The method works well on files with repeating data. On random or previously compressed data, the size after compression does not decrease. The application detects such cases and saves raw unmodified data.

## Version 3.x / GUI Container Version

Advanced hybrid architecture supporting multi-file containers, directory hierarchies, and cross-platform Self-Extracting packages.

### Features

- Hybrid architecture with a high-performance C++ core and Python Tkinter GUI.
- Multi-file and directory container support preserving directory structures.
- Automatic compression optimization.
- Self-Extracting archive generation and automated cross-platform releases via GitHub Actions.

### Build Requirements

- CMake 3.15 or newer
- C++17 compiler (MSVC, GCC, or Clang)
- Python 3.11 or newer
- PyInstaller module
- Inno Setup (for Windows installer building on CI)

### Building and Packaging

Clone the repository:

```bash
git clone https://github.com/Mr-Ordinary-UA/simple-archiver.git
cd simple-archiver
```

Build the C++ core and Python bindings:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### How to Use the GUI Version

Run the application from the python directory:

```bash
cd python
python gui.py
```

- **Compressing files or folders:** Select the compress mode, add individual files or entire folders using the interface buttons, and create the archive.
- **Decompressing an archive:** Switch to decompress mode, select the archive, and choose the target directory for extraction.

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
git clone https://github.com/Mr-Ordinary-UA/simple-archiver.git
cd simple-archiver
```

Build the project:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Executable Output Paths

- Windows (MSVC): `build/Debug/archive.exe`
- Windows (MinGW): `build/archive.exe`
- Linux: `build/archive`

### How to Use Version 1.x

Run the terminal application and select an option from the menu:

1. Compress file
2. Decompress file
3. Exit

- **Compressing a file:** Select option one, enter the source file path, and enter the destination archive path.
- **Decompressing an archive:** Select option two, enter the archive path, and enter the destination file path.

## Pre-built Executables & Installers

Pre-compiled binaries, Windows setup packages, macOS packages, and Linux packages are available in the Releases section of this repository.

## Archive Format Structure

The binary container format begins with a custom signature header, volume indicators, total file count, and individual file metadata headers containing relative paths, directory flags, original sizes, and compression modes, followed by the payload data.

## License

This project is released under the MIT license. The full text is available in the LICENSE file.
