#ifndef CAPY_CHAT_BUNDLED_TOOLS_H
#define CAPY_CHAT_BUNDLED_TOOLS_H


#include <opendht.h>
#include <opendht/log.h>
#include <opendht/crypto.h>
#include <opendht/network_utils.h>
#include <getopt.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <csignal>
#include <unistd.h>

#include <string>
#include <vector>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <sstream>
#include <fstream>

struct dht_params {
    bool daemonize {false};
    bool peer_discovery {false};
    bool log {false};
    bool syslog {false};
    std::string logfile {};
    in_port_t port {0};
    std::string persist_path {};
    dht::crypto::Identity id {};
    std::string privkey_pwd {};
    std::string save_identity {};
};


std::vector<uint8_t> load_file(const std::string& path);

std::pair<dht::DhtRunner::Config, dht::DhtRunner::Context>
        get_dgt_config(dht_params& params);

void print_node_info(const dht::DhtRunner& node, const dht_params& params);

#endif //CAPY_CHAT_BUNDLED_TOOLS_H
