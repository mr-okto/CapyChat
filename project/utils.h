#ifndef CAPY_CHAT_UTILS_H
#define CAPY_CHAT_UTILS_H

#include <opendht.h>

#define BOOTSTRAP_IP "3.121.25.185"
#define TYPE_MSG "message"
#define TYPE_LEFT "user_left"
#define TYPE_CONNECTED "user_connected"

void print_info(dht::DhtRunner &node, const dht::InfoHash &key);
void print_publish_status(bool success);
std::string print_time(const std::time_t& now);

#endif //CAPY_CHAT_UTILS_H
