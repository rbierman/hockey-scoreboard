#include "ResourceLocator.h"
#include <cpplocate/cpplocate.h>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

ResourceLocator::ResourceLocator() {
    // 1. Try local path (development)
    std::string modulePath = cpplocate::getModulePath();
    
    if (!modulePath.empty()) {
        if (fs::exists(modulePath + "/fonts")) {
            _basePath = modulePath;
        } else if (fs::exists(fs::path(modulePath).parent_path().string() + "/fonts")) {
            _basePath = fs::path(modulePath).parent_path().string();
        }
    }
    
    // 2. Try standard installation path
    if (_basePath.empty() && fs::exists("/usr/share/puckpulse-controller/fonts")) {
        _basePath = "/usr/share/puckpulse-controller";
    }

    // 3. Fallback
    if (_basePath.empty()) {
        std::cerr << "Warning: Could not locate resources in local or install paths. Falling back to current directory." << std::endl;
        _basePath = ".";
    }
    
    std::cout << "Resource base path selected: " << _basePath << std::endl;
}

std::string ResourceLocator::getFontsDirPath() const {
    return _basePath + "/fonts";
}

std::string ResourceLocator::getDataDirPath() const {
    // Standard Linux path for mutable application data
    if (fs::exists("/var/lib/puckpulse-controller")) {
        return "/var/lib/puckpulse-controller";
    }
    // Fallback to local development directory
    return _basePath + "/data";
}
