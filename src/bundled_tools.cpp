#include <filesystem>
#include "bundled_tools.h"

std::vector<uint8_t> load_file(const std::string& path) {
    std::vector<uint8_t> buffer {};
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
    bool keychain = std::filesystem::exists(params.save_identity + ".pem")
                    and std::filesystem::exists(params.save_identity + ".crt");
    if (keychain) {
        try {
            params.id.first = std::make_shared<dht::crypto::PrivateKey>(
                    load_file(params.save_identity + ".pem"),
                    params.privkey_pwd);
            params.id.second = std::make_shared<dht::crypto::Certificate>(
                    load_file(params.save_identity + ".crt"));
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error loading RSA keypair: ")
                                     + e.what());
        }
    } else {
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
    config.dht_config.node_config.maintain_storage = true;
    config.dht_config.node_config.persist_path = params.persist_path;
    config.dht_config.id = params.id;
    config.threaded = true;
    config.push_node_id = "dhtnode";
    config.peer_discovery = params.peer_discovery;
    config.peer_publish = params.peer_discovery;

    dht::DhtRunner::Context context {};
    if (params.log) {
        if (params.syslog or params.logfile.empty()) {
            context.logger = dht::log::getSyslogLogger("dhtnode");
        }
        else if (not params.logfile.empty()) {
            context.logger = dht::log::getFileLogger(params.logfile);
        }
        else {
            context.logger = dht::log::getStdLogger();
        }
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

void setup_signals() {
    signal(SIGCHLD,SIG_IGN); /* ignore child */
    signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGHUP,signal_handler); /* catch hangup signal */
    signal(SIGINT,signal_handler); /* catch interrupt signal */
    signal(SIGTERM,signal_handler); /* catch kill signal */
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
