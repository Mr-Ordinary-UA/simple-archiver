#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

std::string getExePath() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return std::string(buffer);
#else
    return std::filesystem::canonical("/proc/self/exe").string();
#endif
}

int main() {
    std::string exePath = getExePath();
    std::ifstream in(exePath, std::ios::binary);
    if (!in) {
        std::cerr << "Помилка читання файлу.\n";
        return 1;
    }

    in.seekg(0, std::ios::end);
    std::streamsize fileSize = in.tellg();
    in.seekg(0, std::ios::beg);

    std::vector<char> buffer(fileSize);
    in.read(buffer.data(), fileSize);

    std::streampos archiveStart = -1;
    for (std::streamsize i = fileSize - 4; i >= 0; --i) {
        if (buffer[i] == 'R' && buffer[i+1] == 'L' && buffer[i+2] == 'E' && buffer[i+3] == '3') {
            uint16_t vol = *reinterpret_cast<uint16_t*>(&buffer[i+4]);
            uint16_t tot = *reinterpret_cast<uint16_t*>(&buffer[i+6]);
            if (vol == 1 && tot == 1) {
                archiveStart = i;
                break;
            }
        }
    }

    if (archiveStart == std::streampos(-1)) {
        std::cerr << "Архів не знайдено.\n";
        return 1;
    }

    in.clear();
    in.seekg(archiveStart + std::streampos(8));

    uint32_t totFiles;
    in.read(reinterpret_cast<char*>(&totFiles), 4);

    std::string outputDir = "extracted_files";
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
                std::vector<char> fileBuf(compSize);
                in.read(fileBuf.data(), compSize);
                out.write(fileBuf.data(), compSize);
            }
        }
    }
    
    std::cout << "Розпаковано у: " << outputDir << "\n";
    return 0;
}