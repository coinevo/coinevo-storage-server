#pragma once

#include <string>
#include <vector>

namespace service_node {
namespace storage {
struct Item;
}
} // namespace service_node

namespace coinevo {

struct message_t;

template <typename T>
void serialize_message(std::string& buf, const T& msg);

template <typename T>
std::vector<std::string> serialize_messages(const std::vector<T>& msgs);

std::vector<message_t> deserialize_messages(const std::string& blob);

} // namespace coinevo
