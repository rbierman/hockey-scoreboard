#pragma once

#include <string>
#include <vector>
#include <cstdint>

class Base64Coder {
public:
    virtual ~Base64Coder() = default;
    virtual std::string encode(const std::vector<uint8_t>& data) const;
    virtual std::vector<uint8_t> decode(const std::string& base64) const;
};
