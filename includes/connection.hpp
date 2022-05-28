#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <array>
#include <memory>
#include <boost/asio.hpp>

#include "../includes/reply.hpp"
#include "../includes/request.hpp"
#include "../includes/request_handler.hpp"
#include "../includes/request_parser.hpp"

namespace http {
namespace server {

class connection_manager;

/// Represents a single connection from a client.
class connection
        : public std::enable_shared_from_this<connection>
{
public:
    connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;

    /// Construct a connection with the given io_service.
    explicit connection(boost::asio::io_service& io_service,
                       request_handler& handler);

    /// Get the socket associated with the connection
    //Connect socket to connection here, not by passing as parameter
    boost::asio::ip::tcp::socket& socket();

    /// Start the first asynchronous operation for the connection.
    void start();

private:
    /// Handle complition of a read operation.
    void handle_read(const boost::system::error_code& e,
                     size_t bytes_transferred);

    /// Handle complition of a write operation.
    void handle_write(const boost::system::error_code& e);

    /// Socket for the connection.
    boost::asio::ip::tcp::socket socket_;

    // The manager for this connection.
    //connection_manager& connection_manager_;

    /// The handler used to process the incloming request.
    request_handler& request_handler_;

    /// Buffer for incloming data.
    std::array<char, 8192> buffer_;

    /// The incoming request.
    request request_;

    /// The parser for the incoming request.
    request_parser request_parser_;

    /// The reply to be sent back to the client.
    reply reply_;

    /// Strand to ensure the connection`s handlers are not called concurrently.
    // A strand is defined as a strictly sequential invocation of event handlers
    // (no concurrent invocation).
    boost::asio::io_service::strand strand_;
};

typedef std::shared_ptr<connection> connection_ptr;

} // namespace server
} // namespace http


#endif // CONNECTION_HPP
