#include "archive_core.h"
#include <fstream>
#include <filesystem>
#include <limits>
#include <cctype>
#include <iterator>

#ifdef _WIN32
#include <windows.h>
#endif

const char SIGNATURE_V1[4] = { 'R', 'L', 'E', '1' };
const char SIGNATURE_V2[4] = { 'R', 'L', 'E', '2' };

double CompressStats::ratio() const
{
    if (originalSize == 0) return 0.0;
    return static_cast<double>(compressedSize) / static_cast<double>(originalSize);
}

std::string normalizePath(const std::string& path)
{
    std::string result = path;
    for (char& c : result)
    {
        if (c == '/') c = '\\';
        c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    }
    return result;
}

bool samePath(const std::string& a, const std::string& b)
{
    return normalizePath(a) == normalizePath(b);
}

#ifdef _WIN32
static std::wstring utf8ToWide(const std::string& utf8)
{
    if (utf8.empty()) return std::wstring();
    int size = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    std::wstring result(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, result.data(), size);
    if (!result.empty() && result.back() == L'\0') result.pop_back();
    return result;
}
#endif

static std::filesystem::path toPath(const std::string& utf8)
{
#ifdef _WIN32
    return std::filesystem::path(utf8ToWide(utf8));
#else
    return std::filesystem::path(utf8);
#endif
}

Result RLE::compress(const std::string& inputFile, const std::string& outputFile, CompressStats& stats)
{
    if (samePath(inputFile, outputFile))
    {
        return Result::SameFile;
    }

    std::filesystem::path inPath = toPath(inputFile);

    std::ifstream in(inPath, std::ios::binary);
    if (!in) return Result::CannotOpenInput;

    std::string data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    if (data.empty()) return Result::EmptyFile;

    std::string encoded;
    encoded.reserve(data.size());

    size_t i = 0;
    while (i < data.size())
    {
        char current = data[i];
        unsigned short count = 1;
        while (i + count < data.size() &&
               data[i + count] == current &&
               count < std::numeric_limits<unsigned short>::max())
        {
            count++;
        }
        encoded.append(reinterpret_cast<char*>(&count), sizeof(count));
        encoded.push_back(current);
        i += count;
    }

    std::ofstream out(toPath(outputFile), std::ios::binary);
    if (!out) return Result::CannotCreateOutput;

    out.write(SIGNATURE_V2, sizeof(SIGNATURE_V2));

    stats.originalSize = data.size();

    unsigned char mode = (encoded.size() < data.size()) ? MODE_COMPRESSED : MODE_RAW;
    out.write(reinterpret_cast<char*>(&mode), 1);

    std::string originalName = inPath.filename().string();
    unsigned short nameLen = static_cast<unsigned short>(originalName.size());
    out.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    out.write(originalName.data(), nameLen);

    if (mode == MODE_COMPRESSED)
    {
        out.write(encoded.data(), static_cast<std::streamsize>(encoded.size()));
        stats.compressedSize = sizeof(SIGNATURE_V2) + 1 + sizeof(nameLen) + nameLen + encoded.size();
    }
    else
    {
        out.write(data.data(), static_cast<std::streamsize>(data.size()));
        stats.compressedSize = sizeof(SIGNATURE_V2) + 1 + sizeof(nameLen) + nameLen + data.size();
    }

    out.close();

    std::error_code ec;
    std::filesystem::remove(inPath, ec);

    return Result::Ok;
}

Result RLE::decompress(const std::string& inputFile, const std::string& outputFile)
{
    if (samePath(inputFile, outputFile))
    {
        return Result::SameFile;
    }

    std::filesystem::path inPath = toPath(inputFile);

    std::ifstream in(inPath, std::ios::binary);
    if (!in) return Result::CannotOpenInput;

    char signature[4];
    if (!in.read(signature, sizeof(signature)))
    {
        return Result::InvalidFormat;
    }

    bool isV1 = true;
    for (int j = 0; j < 4; j++)
    {
        if (signature[j] != SIGNATURE_V1[j]) { isV1 = false; break; }
    }
    bool isV2 = true;
    for (int j = 0; j < 4; j++)
    {
        if (signature[j] != SIGNATURE_V2[j]) { isV2 = false; break; }
    }
    if (!isV1 && !isV2)
    {
        return Result::InvalidFormat;
    }

    unsigned char mode;
    if (!in.read(reinterpret_cast<char*>(&mode), 1))
    {
        return Result::CorruptedArchive;
    }

    if (isV2)
    {
        unsigned short nameLen = 0;
        if (!in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen)))
        {
            return Result::CorruptedArchive;
        }
        in.seekg(nameLen, std::ios::cur);
        if (!in)
        {
            return Result::CorruptedArchive;
        }
    }

    std::ofstream out(toPath(outputFile), std::ios::binary);
    if (!out) return Result::CannotCreateOutput;

    if (mode == MODE_RAW)
    {
        out << in.rdbuf();
        out.close();
        in.close();

        std::error_code ec;
        std::filesystem::remove(inPath, ec);
        return Result::Ok;
    }

    if (mode != MODE_COMPRESSED)
    {
        return Result::InvalidFormat;
    }

    unsigned short count;
    char symbol;
    while (in.read(reinterpret_cast<char*>(&count), sizeof(count)))
    {
        if (!in.read(&symbol, 1))
        {
            return Result::CorruptedArchive;
        }
        for (unsigned short k = 0; k < count; k++)
        {
            out.put(symbol);
        }
    }

    out.close();
    in.close();

    std::error_code ec;
    std::filesystem::remove(inPath, ec);

    return Result::Ok;
}

std::string peekOriginalName(const std::string& archiveFile)
{
    std::ifstream in(toPath(archiveFile), std::ios::binary);
    if (!in) return "";

    char signature[4];
    if (!in.read(signature, sizeof(signature))) return "";

    bool isV2 = true;
    for (int j = 0; j < 4; j++)
    {
        if (signature[j] != SIGNATURE_V2[j]) { isV2 = false; break; }
    }
    if (!isV2) return "";

    unsigned char mode;
    if (!in.read(reinterpret_cast<char*>(&mode), 1)) return "";

    unsigned short nameLen = 0;
    if (!in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen))) return "";

    std::string name(nameLen, '\0');
    if (!in.read(name.data(), nameLen)) return "";

    return name;
}

std::string resultToString(Result r)
{
    switch (r)
    {
        case Result::Ok: return "Success";
        case Result::CannotOpenInput: return "Cannot open input file";
        case Result::CannotCreateOutput: return "Cannot create output file";
        case Result::EmptyFile: return "Input file is empty";
        case Result::InvalidFormat: return "File is not a valid RLE archive";
        case Result::CorruptedArchive: return "Archive is corrupted";
        case Result::SameFile: return "Input and output paths must differ";
    }
    return "Unknown result";
}