#pragma once

#include <cstdint>
#include <string>
#include <vector>

std::vector<uint8_t> parseCoinevodKey(const std::string& path);

std::vector<uint8_t> calcPublicKey(const std::vector<uint8_t>& private_key);
