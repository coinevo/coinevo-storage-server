#include "channel_encryption.hpp"
#include "http_connection.h"
#include "coinevod_key.h"
#include "service_node.h"
#include "swarm.h"

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/program_options.hpp>

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <utility> // for std::pair
#include <vector>

using namespace service_node;
namespace po = boost::program_options;
namespace logging = boost::log;

using LogLevelPair = std::pair<std::string, logging::trivial::severity_level>;
using LogLevelMap = std::vector<LogLevelPair>;
static const LogLevelMap logLevelMap{
    {"trace", logging::trivial::severity_level::trace},
    {"debug", logging::trivial::severity_level::debug},
    {"info", logging::trivial::severity_level::info},
    {"warning", logging::trivial::severity_level::warning},
    {"error", logging::trivial::severity_level::error},
    {"fatal", logging::trivial::severity_level::fatal},
};

void usage(char* argv[]) {
    std::cerr << "Usage: " << argv[0]
              << " <address> <port> [--coinevod-key path] [--db-location "
                 "path] [--log-level level]\n";
    std::cerr << "  For IPv4, try:\n";
    std::cerr << "    receiver 0.0.0.0 80\n";
    std::cerr << "  For IPv6, try:\n";
    std::cerr << "    receiver 0::0 80\n";
    std::cerr << "  Log levels:\n";
    for (const auto& logLevel : logLevelMap) {
        std::cerr << "    " << logLevel.first << "\n";
    }
}

bool parseLogLevel(const std::string& input,
                   logging::trivial::severity_level& logLevel) {

    const auto it = std::find_if(
        logLevelMap.begin(), logLevelMap.end(),
        [&](const LogLevelPair& pair) { return pair.first == input; });
    if (it != logLevelMap.end()) {
        logLevel = it->second;
        return true;
    }
    return false;
}

int main(int argc, char* argv[]) {
    try {
        // Check command line arguments.
        if (argc < 3) {
            usage(argv);
            return EXIT_FAILURE;
        }

        std::string coinevodKeyPath;
        std::string dbLocation(".");
        std::string logLocation;
        std::string logLevelString("info");

        const auto port = static_cast<uint16_t>(std::atoi(argv[2]));
        std::string ip = argv[1];

        po::options_description desc;
        desc.add_options()("coinevod-key", po::value(&coinevodKeyPath),
                           "")("db-location", po::value(&dbLocation),
                               "")("output-log", po::value(&logLocation), "")(
            "log-level", po::value(&logLevelString), "");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        if (vm.count("output-log")) {
            auto sink = logging::add_file_log(logLocation + ".out");
            sink->locked_backend()->auto_flush(true);
            BOOST_LOG_TRIVIAL(info)
                << "Outputting logs to " << logLocation << ".out";
        }

        logging::trivial::severity_level logLevel;
        if (!parseLogLevel(logLevelString, logLevel)) {
            BOOST_LOG_TRIVIAL(error) << "Incorrect log level" << logLevelString;
            usage(argv);
            return EXIT_FAILURE;
        }

        // TODO: consider adding auto-flushing for logging
        logging::core::get()->set_filter(logging::trivial::severity >=
                                         logLevel);
        BOOST_LOG_TRIVIAL(info) << "Setting log level to " << logLevelString;

        if (vm.count("coinevod-key")) {
            BOOST_LOG_TRIVIAL(info)
                << "Setting Coinevod key path to " << coinevodKeyPath;
        }

        if (vm.count("db-location")) {
            BOOST_LOG_TRIVIAL(info)
                << "Setting database location to " << dbLocation;
        }

        BOOST_LOG_TRIVIAL(info)
            << "Listening at address " << ip << " port " << port << std::endl;

        boost::asio::io_context ioc{1};

        // ed25519 key
        const std::vector<uint8_t> private_key = parseCoinevodKey(coinevodKeyPath);
        ChannelEncryption<std::string> channelEncryption(private_key);
        const std::vector<uint8_t> public_key = calcPublicKey(private_key);
        coinevo::ServiceNode service_node(ioc, port, public_key, dbLocation);

        /// Should run http server
        coinevo::http_server::run(ioc, ip, port, service_node, channelEncryption);

    } catch (std::exception const& e) {
        BOOST_LOG_TRIVIAL(fatal) << "Exception caught in main: " << e.what();
        return EXIT_FAILURE;
    }
}
