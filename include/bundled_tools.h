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

static const constexpr char* PROMPT = ">> ";

struct dht_params {
    bool help {false}; // print help and exit
    bool version {false};
    bool generate_identity {false};
    bool daemonize {false};
    bool service {false};
    bool peer_discovery {false};
    bool log {false};
    bool syslog {false};
    std::string logfile {};
    std::pair<std::string, std::string> bootstrap {};
    dht::NetId network {0};
    in_port_t port {0};
    in_port_t proxyserver {0};
    in_port_t proxyserverssl {0};
    std::string proxyclient {};
    std::string pushserver {};
    std::string devicekey {};
    std::string persist_path {};
    dht::crypto::Identity id {};
    dht::crypto::Identity proxy_id {};
    std::string privkey_pwd {};
    std::string proxy_privkey_pwd {};
    std::string save_identity {};
    bool no_rate_limit {false};
};

static const constexpr struct option long_options[] = {
        {"help",                    no_argument      , nullptr, 'h'},
        {"port",                    required_argument, nullptr, 'p'},
        {"net",                     required_argument, nullptr, 'n'},
        {"bootstrap",               required_argument, nullptr, 'b'},
        {"identity",                no_argument      , nullptr, 'i'},
        {"save-identity",           required_argument, nullptr, 'I'},
        {"certificate",             required_argument, nullptr, 'c'},
        {"privkey",                 required_argument, nullptr, 'k'},
        {"privkey-password",        required_argument, nullptr, 'm'},
        {"verbose",                 no_argument      , nullptr, 'v'},
        {"daemonize",               no_argument      , nullptr, 'd'},
        {"service",                 no_argument      , nullptr, 's'},
        {"peer-discovery",          no_argument      , nullptr, 'D'},
        {"no-rate-limit",           no_argument      , nullptr, 'U'},
        {"persist",                 required_argument, nullptr, 'f'},
        {"logfile",                 required_argument, nullptr, 'l'},
        {"syslog",                  no_argument      , nullptr, 'L'},
        {"proxyserver",             required_argument, nullptr, 'S'},
        {"proxyserverssl",          required_argument, nullptr, 'e'},
        {"proxy-certificate",       required_argument, nullptr, 'w'},
        {"proxy-privkey",           required_argument, nullptr, 'K'},
        {"proxy-privkey-password",  required_argument, nullptr, 'M'},
        {"proxyclient",             required_argument, nullptr, 'C'},
        {"pushserver",              required_argument, nullptr, 'y'},
        {"devicekey",               required_argument, nullptr, 'z'},
        {"version",                 no_argument      , nullptr, 'V'},
        {nullptr,                   0                , nullptr,  0}
};


// The mapString shall have the following format:  k1:v1[,k2:v2[,...]]
std::map<std::string, std::string> parse_string_map(const std::string& map_string);

bool is_info_hash(const dht::InfoHash& h);

std::vector<uint8_t> load_file(const std::string& path);

std::pair<dht::DhtRunner::Config, dht::DhtRunner::Context>
        get_dgt_config(dht_params& params);

void print_node_info(const dht::DhtRunner& node, const dht_params& params);

dht_params parse_args(int argc, char** argv);

std::string readLine(const char* prefix = PROMPT);

class ServiceRunner {
public:
    bool wait();
    void kill();
private:
    std::condition_variable cv;
    std::mutex m;
    std::atomic_bool terminate {false};
};

ServiceRunner sig_runner;

void signal_handler(int sig);

void setupSignals();

void daemonize();

#endif //CAPY_CHAT_BUNDLED_TOOLS_H
