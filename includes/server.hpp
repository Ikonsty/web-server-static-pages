#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include <string>

#include "../includes/connection.hpp"
//#include "../includes/connection_manager.hpp"
#include "../includes/request_handler.hpp"


namespace http {
namespace server {

class server
{
public:
    server(const server&) = delete;
    server& operator=(const server&) = delete;

    /// Construct the server to listen on the specified TCP address and port, and
    /// serve up files from the given directory.
    explicit server(const std::string& address, const std::string& port,
                    const std::string& doc_root, size_t thread_pool_size);

    /// Run the server`s io_servise loop
    void run();

private:
    /// Start an asynchronous accept operation.
    void start_accept();

    /// Handle completion of an asynchronous accept.
    void handle_accept(const boost::system::error_code& e);

    /// Handle a request to stop the server.
    void handle_stop();

    /// The number of threads that can call io_sercive:::run().
    size_t thread_pool_size_;

    /// The io_service used to perform asynchronous operations.
    boost::asio::io_service io_service_;

    /// The signal_set is used to register for process termination notifications.
    boost::asio::signal_set signals_;

    /// Acceptor listen for incloming connections.
    boost::asio::ip::tcp::acceptor acceptor_;

    // The connection manager which owns all live connections.
    //connection_manager connection_manager_;

    // The next socket to be accepted.
    //boost::asio::ip::tcp::socket socket_;

    /// Next connection to be accepted.
    connection_ptr new_connection_;

    /// The handler for all incloming requests.
    request_handler request_handler_;
};

} // namespace server
} // namespace http

#endif // SERVER_HPP
