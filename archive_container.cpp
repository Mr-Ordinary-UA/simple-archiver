#include "archive_container.h"
#include <fstream>
#include <filesystem>
#include <iterator>
#include <limits>

Result packFilesRaw(const std::vector<std::pair<std::string, std::string>>& files, const std::string& archivePath) {
    std::ofstream out(archivePath, std::ios::binary);
    if (!out) return Result::CannotCreateOutput;

    char sig[4] = {'R', 'L', 'E', '3'};
    out.write(sig, 4);

    uint16_t volIdx = 1;
    uint16_t totVol = 1;
    uint32_t totFiles = static_cast<uint32_t>(files.size());

    out.write(reinterpret_cast<const char*>(&volIdx), 2);
    out.write(reinterpret_cast<const char*>(&totVol), 2);
    out.write(reinterpret_cast<const char*>(&totFiles), 4);

    for (const auto& item : files) {
        std::filesystem::path p = item.first;
        std::string name = item.second;
        bool isDir = std::filesystem::is_directory(p);
        uint16_t nameLen = static_cast<uint16_t>(name.size());
        uint8_t isDirFlag = isDir ? 1 : 0;
        
        uint64_t origSize = 0;
        uint64_t compSize = 0;
        uint8_t compMethod = 0;

        std::string rawData;
        std::string encodedData;

        if (!isDir) {
            origSize = std::filesystem::file_size(p);
            std::ifstream in(p, std::ios::binary);
            if (in) {
                rawData.assign((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
                in.close();

                encodedData.reserve(rawData.size());
                size_t i = 0;
                while (i < rawData.size()) {
                    char current = rawData[i];
                    unsigned short count = 1;
                    while (i + count < rawData.size() &&
                           rawData[i + count] == current &&
                           count < std::numeric_limits<unsigned short>::max()) {
                        count++;
                    }
                    encodedData.append(reinterpret_cast<char*>(&count), sizeof(count));
                    encodedData.push_back(current);
                    i += count;
                }

                if (encodedData.size() < rawData.size()) {
                    compMethod = 1;
                    compSize = encodedData.size();
                } else {
                    compMethod = 0;
                    compSize = rawData.size();
                }
            }
        }

        out.write(reinterpret_cast<const char*>(&nameLen), 2);
        out.write(name.data(), nameLen);
        out.write(reinterpret_cast<const char*>(&isDirFlag), 1);
        out.write(reinterpret_cast<const char*>(&origSize), 8);
        out.write(reinterpret_cast<const char*>(&compSize), 8);
        out.write(reinterpret_cast<const char*>(&compMethod), 1);

        if (!isDir) {
            if (compMethod == 1) {
                out.write(encodedData.data(), encodedData.size());
            } else {
                out.write(rawData.data(), rawData.size());
            }
        }
    }

    out.close();
    if (!out) return Result::CannotCreateOutput;

    for (auto it = files.rbegin(); it != files.rend(); ++it) {
        std::error_code ec;
        std::filesystem::remove(it->first, ec);
    }

    return Result::Ok;
}

Result unpackFilesRaw(const std::string& archivePath, const std::string& outputDir) {
    std::ifstream in(archivePath, std::ios::binary);
    if (!in) return Result::CannotOpenInput;

    char sig[4];
    in.read(sig, 4);
    if (sig[0] != 'R' || sig[1] != 'L' || sig[2] != 'E' || sig[3] != '3') {
        return Result::InvalidFormat;
    }

    uint16_t volIdx, totVol;
    uint32_t totFiles;
    in.read(reinterpret_cast<char*>(&volIdx), 2);
    in.read(reinterpret_cast<char*>(&totVol), 2);
    in.read(reinterpret_cast<char*>(&totFiles), 4);

    std::filesystem::create_directories(outputDir);

    for (uint32_t i = 0; i < totFiles; ++i) {
        uint16_t nameLen;
        in.read(reinterpret_cast<char*>(&nameLen), 2);
        std::string name(nameLen, '\0');
        in.read(name.data(), nameLen);

        uint8_t isDirFlag;
        uint64_t origSize, compSize;
        uint8_t compMethod;
        in.read(reinterpret_cast<char*>(&isDirFlag), 1);
        in.read(reinterpret_cast<char*>(&origSize), 8);
        in.read(reinterpret_cast<char*>(&compSize), 8);
        in.read(reinterpret_cast<char*>(&compMethod), 1);

        std::filesystem::path outPath = std::filesystem::path(outputDir) / name;
        
        if (outPath.has_parent_path()) {
            std::filesystem::create_directories(outPath.parent_path());
        }

        if (isDirFlag) {
            std::filesystem::create_directories(outPath);
        } else {
            std::ofstream out(outPath, std::ios::binary);
            if (!out) return Result::CannotCreateOutput;
            
            if (compMethod == 1) {
                uint64_t bytesRead = 0;
                unsigned short count;
                char symbol;
                while (bytesRead < compSize) {
                    in.read(reinterpret_cast<char*>(&count), sizeof(count));
                    in.read(&symbol, 1);
                    bytesRead += sizeof(count) + 1;
                    for (unsigned short k = 0; k < count; k++) {
                        out.put(symbol);
                    }
                }
            } else {
                std::vector<char> buffer(compSize);
                in.read(buffer.data(), compSize);
                out.write(buffer.data(), compSize);
            }
        }
    }
    return Result::Ok;
}