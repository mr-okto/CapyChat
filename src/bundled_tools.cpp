#include "bundled_tools.h"

std::map<std::string, std::string> parse_string_map(
        const std::string &map_string) {
    std::istringstream keySs(map_string);
    std::string mapStr;
    std::map<std::string, std::string> map;

    while (std::getline(keySs, mapStr, ',')) {
        std::istringstream mapSs(mapStr);
        std::string key, value;

        while (std::getline(mapSs, key, ':')) {
            std::getline(mapSs, value, ':');
            map[key] = { value };
        }
    }
    return map;
}

bool is_info_hash(const dht::InfoHash& h) {
    if (not h) {
        std::cout << "Syntax error: invalid InfoHash." << std::endl;
        return false;
    }
    return true;
}

std::vector<uint8_t> load_file(const std::string& path) {
    std::vector<uint8_t> buffer;
    std::ifstream file(path, std::ios::binary);
    if (!file)
        throw std::runtime_error("Can't read file: "+path);
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    if (size > std::numeric_limits<unsigned>::max())
        throw std::runtime_error("File is too big: "+path);
    buffer.resize(size);
    file.seekg(0, std::ios::beg);
    if (!file.read((char*)buffer.data(), size))
        throw std::runtime_error("Can't load file: "+path);
    return buffer;
}

std::pair<dht::DhtRunner::Config, dht::DhtRunner::Context>
        get_dgt_config(dht_params& params) {
    if (not params.id.first and params.generate_identity) {
        auto node_ca = std::make_unique<dht::crypto::Identity>(
                dht::crypto::generateEcIdentity("DHT Node CA"));

        params.id = dht::crypto::generateIdentity("DHT Node", *node_ca);
        if (not params.save_identity.empty()) {
            dht::crypto::saveIdentity(*node_ca, params.save_identity + "_ca",
                                      params.privkey_pwd);
            dht::crypto::saveIdentity(params.id, params.save_identity,
                                      params.privkey_pwd);
        }
    }

    dht::DhtRunner::Config config {};
    config.dht_config.node_config.network = params.network;
    config.dht_config.node_config.maintain_storage = false;
    config.dht_config.node_config.persist_path = params.persist_path;
    config.dht_config.id = params.id;
    config.threaded = true;
    config.proxy_server = params.proxyclient;
    config.push_node_id = "dhtnode";
    config.push_token = params.devicekey;
    config.peer_discovery = params.peer_discovery;
    config.peer_publish = params.peer_discovery;
    if (params.no_rate_limit) {
        config.dht_config.node_config.max_req_per_sec = -1;
        config.dht_config.node_config.max_peer_req_per_sec = -1;
    }

    dht::DhtRunner::Context context {};
    if (params.log) {
        if (params.syslog or (params.daemonize and params.logfile.empty()))
            context.logger = dht::log::getSyslogLogger("dhtnode");
        else if (not params.logfile.empty())
            context.logger = dht::log::getFileLogger(params.logfile);
        else
            context.logger = dht::log::getStdLogger();
    }
    if (context.logger) {
        context.statusChangedCallback =
                [logger = *context.logger](dht::NodeStatus status4,
                                           dht::NodeStatus status6) {
            logger.WARN("Connectivity changed: IPv4: %s, IPv6: %s",
                        dht::statusToStr(status4),
                        dht::statusToStr(status6));
        };
    }
    return {std::move(config), std::move(context)};
}

void print_node_info(const dht::DhtRunner &node, const dht_params &params) {
    std::cout << "OpenDHT node " << node.getNodeId() << " running on ";
    auto port4 = node.getBoundPort(AF_INET);
    auto port6 = node.getBoundPort(AF_INET6);
    if (port4 == port6)
        std::cout << "port " << port4 << std::endl;
    else
        std::cout << "IPv4 port " << port4 << ", IPv6 port " << port6 << std::endl;
    if (params.id.first)
        std::cout << "Public key ID " << node.getId() << std::endl;
}

dht_params parse_args(int argc, char **argv) {
    dht_params params {};
    int opt;
    std::string privkey;
    std::string proxy_privkey;
    while ((opt = getopt_long(argc, argv, "hidsvDUp:n:b:f:l:", long_options,
                              nullptr)) != -1) {
        switch (opt) {
            case 'p': {
                int port_arg = atoi(optarg);
                if (port_arg >= 0 && port_arg < 0x10000)
                    params.port = port_arg;
                else
                    std::cout << "Invalid port: " << port_arg << std::endl;
            }
                break;
            case 'S': {
                int port_arg = atoi(optarg);
                if (port_arg >= 0 && port_arg < 0x10000)
                    params.proxyserver = port_arg;
                else
                    std::cout << "Invalid port: " << port_arg << std::endl;
            }
                break;
            case 'e': {
                int port_arg = atoi(optarg);
                if (port_arg >= 0 && port_arg < 0x10000)
                    params.proxyserverssl = port_arg;
                else
                    std::cout << "Invalid port: " << port_arg << std::endl;
            }
                break;
            case 'D':
                params.peer_discovery = true;
                break;
            case 'y':
                params.pushserver = optarg;
                break;
            case 'C':
                params.proxyclient = optarg;
                break;
            case 'z':
                params.devicekey = optarg;
                break;
            case 'f':
                params.persist_path = optarg;
                break;
            case 'n':
                params.network = strtoul(optarg, nullptr, 0);
                break;
            case 'U':
                params.no_rate_limit = true;
                break;
            case 'b':
                params.bootstrap = dht::splitPort((optarg[0] == '=') ?
                        optarg + 1 : optarg);
                if (not params.bootstrap.first.empty() and
                        params.bootstrap.second.empty()) {
                    params.bootstrap.second = std::to_string(dht::net::DHT_DEFAULT_PORT);
                }
                break;
            case 'V':
                params.version = true;
                break;
            case 'h':
                params.help = true;
                break;
            case 'l':
                params.logfile = optarg;
                break;
            case 'L':
                params.log = true;
                params.syslog = true;
                break;
            case 'v':
                params.log = true;
                break;
            case 'i':
                params.generate_identity = true;
                break;
            case 'd':
                params.daemonize = true;
                break;
            case 's':
                params.service = true;
                break;
            case 'c': {
                try {
                    params.id.second = std::make_shared<dht::crypto::Certificate>(
                            load_file(optarg));
                } catch (const std::exception& e) {
                    throw std::runtime_error(
                            std::string("Error loading certificate: ") + e.what());
                }
                break;
            }
            case 'w': {
                try {
                    params.proxy_id.second =
                            std::make_shared<dht::crypto::Certificate>(
                                    load_file(optarg));
                } catch (const std::exception& e) {
                    throw std::runtime_error(
                            std::string("Error loading proxy certificate: ")
                            + e.what());
                }
                break;
            }
            case 'k':
                privkey = optarg;
                break;
            case 'K':
                proxy_privkey = optarg;
                break;
            case 'm':
                params.privkey_pwd = optarg;
                break;
            case 'M':
                params.proxy_privkey_pwd = optarg;
                break;
            case 'I':
                params.save_identity = optarg;
                break;
            default:
                break;
        }
    }
    if (not privkey.empty()) {
        try {
            params.id.first = std::make_shared<dht::crypto::PrivateKey>(
                    load_file(privkey), params.privkey_pwd);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string(
                    "Error loading private key: ") + e.what());
        }
    }
    if (not proxy_privkey.empty()) {
        try {
            params.proxy_id.first = std::make_shared<dht::crypto::PrivateKey>(
                    load_file(proxy_privkey), params.proxy_privkey_pwd);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string(
                    "Error loading proxy private key: ") + e.what());
        }
    }
    if (params.save_identity.empty())
        params.privkey_pwd.clear();
    return params;
}

std::string read_line(const char *prefix) {
    const char* line_read = readline(prefix);
    if (line_read && *line_read)
        add_history(line_read);
    return line_read ? std::string(line_read) : std::string("\0", 1);
}

ServiceRunner sig_runner;

void signal_handler(int sig) {
    switch(sig) {
        case SIGHUP:
            break;
        case SIGINT:
            close(STDIN_FILENO);
            // fall through
        case SIGTERM:
            sig_runner.kill();
            break;
        default:
            break;
    }
}

void setupSignals() {
    signal(SIGCHLD,SIG_IGN); /* ignore child */
    signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGHUP,signal_handler); /* catch hangup signal */
    signal(SIGINT,signal_handler); /* catch interrupt signal */
    signal(SIGTERM,signal_handler); /* catch kill signal */
}

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);

    pid_t sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

bool ServiceRunner::wait() {
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, [&]{return terminate.load();});
    return !terminate;
}

void ServiceRunner::kill() {
    terminate = true;
    cv.notify_all();
}
