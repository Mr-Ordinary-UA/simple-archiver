# simple-archiver

A simple file archiver based on the RLE (Run-Length Encoding) algorithm. This project serves as a portfolio piece demonstrating file handling, binary data manipulation, and error processing in C++.

A console version is currently available. A graphical interface is planned.

## RLE Description

RLE is a data compression method that replaces sequences of identical bytes with a pair of values: a repeat count and the byte itself. A sequence of ten identical characters takes less space in compressed form than in the original.

The method works well on files with a large amount of repeating data, such as simple images or repetitive texts. On random or already compressed data (archives, JPEG photos), the size after compression might not decrease. In this case, the program saves the file without compression and marks it as raw.

## Program Features

- Compresses a file into a custom archive format
- Decompresses the archive back into the original file
- Shows the size before and after compression, along with the compression ratio
- Verifies archive integrity before decompression
- Deletes the original file after successful compression or decompression

## Build Requirements

- CMake version 3.15 or newer
- Any C++ compiler supporting the C++17 standard: MSVC (Visual Studio), MinGW (g++), or Clang

Check if CMake is installed:

```bash
cmake --version
```

If the command is not found, install CMake from the official website: [cmake.org](https://cmake.org).

## Building the Project

Clone the repository:

```bash
git clone https://github.com/Mr-Ordinary-UA/simple-archiver.git
cd simple-archiver
```

Create a build folder and navigate into it:

```bash
mkdir build
cd build
```

Generate the build files. This command finds the compiler installed on your system and prepares the necessary files for it:

```bash
cmake ..
```

Build the project:

```bash
cmake --build .
```

After completion, the executable appears inside the `build` folder.

| Environment | Executable path |
|---|---|
| Windows, Visual Studio | `build/Debug/archive.exe` |
| Windows, MinGW | `build/archive.exe` |
| Linux | `build/archive` |

## Pre-built Executable

If you do not want to build the project yourself, a ready-made Windows executable is available in the [Releases](https://github.com/Mr-Ordinary-UA/simple-archiver/releases) section of this repository. Download the latest version and run it directly.

## How to Use the Program

After launch, a menu with three options opens:

```
1. Compress file
2. Decompress file
3. Exit
```

### Compressing a file

1. Select option `1`.
2. Enter the path to the file you want to compress.
3. Enter the path to save the archive, for example `photo.rle`.

The program shows the size before and after compression, along with the compression ratio. The original file is deleted after successful compression.

### Decompressing a file

1. Select option `2`.
2. Enter the path to the archive.
3. Enter the path to save the restored file.

The archive is deleted after successful decompression.

Paths can be relative (`photo.jpg`, if the file is next to the program) or absolute (`C:\Users\Name\Desktop\photo.jpg`).

## Archive Format

The archive file begins with the `RLE1` signature, followed by one byte indicating the mode: compressed data or raw (unmodified) data. This lets the program verify, during decompression, that the file is a valid archive of this format, and select the correct data restoration method.

## License

This project is distributed under the MIT license. The full text is available in the [LICENSE](./LICENSE) file.
