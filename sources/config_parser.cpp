#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include <iterator>
#include "../includes/errors.hpp"
#include "../includes/config_parser.hpp"

namespace po = boost::program_options;
namespace sys = std::filesystem;

config_options_t::config_options_t() {
    config.add_options()
            ("address", po::value<std::string>()->default_value(""), "address")
            ("port", po::value<std::string>()->default_value(""), "port")
            ("doc_root", po::value<sys::path>()->default_value(""), "doc_root")
            ("threads", po::value<size_t>()->default_value(1), "threads");
}

config_options_t::config_options_t(char *argv[]):
        config_options_t()
{
    parse(argv);
}

void config_options_t::parse(char *argv[]) {
    std::string config_file = argv[1];
    std::ifstream file(config_file.c_str());
    if (!file)
    {
        std::cerr << "Can`t open config file" << std::endl;
        exit(CFG_FILE_OPEN_ERROR);
    }
    try {
        po::options_description config_file_options;
        config_file_options.add(config);
        store(parse_config_file(file, config_file_options), vm);
        notify(vm);
        address = vm["address"].as<std::string>();
        port = vm["port"].as<std::string>();
        doc_root = vm["doc_root"].as<sys::path>();
        threads = vm["threads"].as<size_t>();

    }
    catch (std::exception &e) {
        {
            std::cerr << e.what() << std::endl;
            exit(CFG_FILE_READ_ERROR);
        }
    }
}
