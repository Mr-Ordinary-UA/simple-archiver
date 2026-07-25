#pragma once
#include <string>

const unsigned char MODE_COMPRESSED = 0;
const unsigned char MODE_RAW = 1;

enum class Result
{
    Ok,
    CannotOpenInput,
    CannotCreateOutput,
    EmptyFile,
    InvalidFormat,
    CorruptedArchive,
    SameFile
};

struct CompressStats
{
    unsigned long long originalSize = 0;
    unsigned long long compressedSize = 0;

    double ratio() const;
};

std::string normalizePath(const std::string& path);
bool samePath(const std::string& a, const std::string& b);

class RLE
{
public:
    Result compress(const std::string& inputFile, const std::string& outputFile, CompressStats& stats);
    Result decompress(const std::string& inputFile, const std::string& outputFile);
};

std::string peekOriginalName(const std::string& archiveFile);

std::string resultToString(Result r);