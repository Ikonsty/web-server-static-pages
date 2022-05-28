#include <iostream>
#include <string>
#include <boost/asio.hpp>

#include "./includes/server.hpp"
#include "./includes/config_parser.hpp"
#include "./includes/errors.hpp"
#include "./includes/help_functions.hpp"

namespace sys = std::filesystem;


int main(int argc, char* argv[])
{
  try
  {
    // Check command line arguments.
    std::cout << "Starting server...\n" << std::endl;

    if (argc != 2)
    {
      std::cerr << "Usage: web_server <pass_to_config_file>" << std::endl;
      exit(ARGUMENTS_ERROR);
    }
    config_options_t obj(argv);

    sys::path doc_root = obj.doc_root;
    std::string address = obj.address;
    std::string port = obj.port;
    size_t threads = obj.threads;

    // Test config parameters block
    if (!sys::exists(doc_root)) {
        std::cerr << "Error: Root directory does not exist" << std::endl;
        exit(CFG_DOES_NOT_EXIST);
    }
    boost::system::error_code ec;
    boost::asio::ip::address::from_string(address, ec);
    if (ec) {
        std::cerr << "Error: Incorrect IP address" << std::endl;
        exit(INCORRECT_ADDRESS);
    }
    if (!is_number(port) || std::stoi(port) > 65353 || std::stoi(port) < 0) {
        std::cerr << "Error: Incorrect port" << std::endl;
        exit(INCORRECT_PORT);
    }
    // End of test block


    // Initialise the server.
    std::cout << "Server launched at: " << address << ":" << port << std::endl;
    std::cout << "Server root directory: " << doc_root << std::endl;
    std::cout << "Max threads: " << threads << std::endl;

    http::server::server s(address, port, doc_root, threads);

    // Run the server until stopped.
    s.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
