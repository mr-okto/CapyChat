#ifndef CAPY_CHAT_UTILS_H
#define CAPY_CHAT_UTILS_H

#include <opendht.h>

void print_info(dht::DhtRunner &node);
void print_publish_status(bool success);
std::string print_time(const std::time_t& now);

#endif //CAPY_CHAT_UTILS_H
