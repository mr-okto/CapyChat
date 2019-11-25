#include "utils.h"

void print_info(dht::DhtRunner& node, const dht::InfoHash& key) {
    std::cout << "Node Stats: " << node.getNodeInfo().ipv4.toString();
    std::cout << "Storage at " << TEST_KEY << ": " << node.getStorageLog(key)
              << std::endl;
    std::cout << "Known public addresses: [ ";
    for (const dht::SockAddr& addr : node.getPublicAddress()) {
        std::cout << addr.toString() << " ";
    }
    std::cout << "]" << std::endl;
}

void print_publish_status(bool success) {
    if (not success) {
        std::cout << "Message publishing failed !" << std::endl;
    }
}

std::string print_time(const std::time_t& now) {
    char buf[80];
    strftime(buf, sizeof(buf), "%X", localtime(&now));
    return buf;
}
