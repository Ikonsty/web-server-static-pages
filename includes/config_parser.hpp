#ifndef QUEUE_PARSER_H
#define QUEUE_PARSER_H
#include <boost/program_options.hpp>
#include <filesystem>

class config_options_t
{
public:
    config_options_t();
    config_options_t(char *argv[]);

    config_options_t(const config_options_t &) = default;
    config_options_t &operator=(const config_options_t &) = delete;
    config_options_t(config_options_t &&) = default;
    config_options_t &operator=(config_options_t &&) = delete;
    ~config_options_t() = default;

    void parse(char *argv[]);

    boost::program_options::variables_map vm;
    boost::program_options::options_description config;
    std::string address;
    std::string port;
    std::filesystem::path doc_root;
    size_t threads;
};

#endif // QUEUE_PARSER_H
