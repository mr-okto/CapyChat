#ifndef CAPY_CHAT_UTILS_H
#define CAPY_CHAT_UTILS_H

#include <opendht.h>

#define TEST_KEY "msg_key"

void print_info(dht::DhtRunner &node, const dht::InfoHash &key);


#endif //CAPY_CHAT_UTILS_H
