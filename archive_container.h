#pragma once
#include "archive_core.h"
#include <string>
#include <vector>
#include <utility>
#include <cstdint>

Result packFilesRaw(const std::vector<std::pair<std::string, std::string>>& files, const std::string& archivePath);
Result unpackFilesRaw(const std::string& archivePath, const std::string& outputDir);