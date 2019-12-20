#ifndef CAPY_CHAT_DHT_CONFIG_H
#define CAPY_CHAT_DHT_CONFIG_H


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
    bool peer_discovery {false};
    bool log {false};
    bool syslog {false};
    std::string logfile {};
    std::pair<std::string, std::string> bootstrap {};
    dht::NetId network {0};
    in_port_t port {0};
    std::string persist_path {};
    dht::crypto::Identity id {};
    dht::crypto::Identity proxy_id {};
    std::string privkey_pwd {};
    std::string proxy_privkey_pwd {};
    std::string save_identity {};
};

std::vector<uint8_t> load_file(const std::string& path);

std::pair<dht::DhtRunner::Config, dht::DhtRunner::Context>
        get_dgt_config(dht_params& params);

void print_node_info(const dht::DhtRunner& node, const dht_params& params);


class ServiceRunner {
public:
    bool wait();
    void kill();
private:
    std::condition_variable cv;
    std::mutex m;
    std::atomic_bool terminate {false};
};

void signal_handler(int sig);

void setup_signals();

void daemonize();

#endif //CAPY_CHAT_DHT_CONFIG_H
