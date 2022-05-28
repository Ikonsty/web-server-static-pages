#include <vector>
#include <boost/bind.hpp>

#include "../includes/connection.hpp"
#include "../includes/request_handler.hpp"

namespace http {
namespace server {

connection::connection(boost::asio::io_service& io_service,
                       request_handler& handler)
    : strand_(io_service),
      socket_(io_service),
      request_handler_(handler)
{
}

boost::asio::ip::tcp::socket& connection::socket() {
    return socket_;
}

void connection::start() {
    socket_.async_read_some(boost::asio::buffer(buffer_),
                            strand_.wrap(                   //create a new handler function object that, when invoked, will automatically pass the wrapped handler to the strand's dispatch function
                                boost::bind(&connection::handle_read, shared_from_this(),
                                     boost::asio::placeholders::error,
                                     boost::asio::placeholders::bytes_transferred)));
}

void connection::handle_read(const boost::system::error_code& e,
                             size_t bytes_transferred) {
    if (!e) {
        boost::tribool result;
        boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
                    request_, buffer_.data(), buffer_.data() + bytes_transferred);

        if (result) {
            request_handler_.handle_request(request_, reply_);
            boost::asio::async_write(socket_, reply_.to_buffers(),
                                     strand_.wrap(
                                         boost::bind(&connection::handle_write, shared_from_this(),
                                                     boost::asio::placeholders::error)));
        } else if (!result) {
            reply_ = reply::stock_reply(reply::bad_request);
            boost::asio::async_write(socket_, reply_.to_buffers(),
                      strand_.wrap(
                        boost::bind(&connection::handle_write, shared_from_this(),
                          boost::asio::placeholders::error)));
        } else {
            socket_.async_read_some(boost::asio::buffer(buffer_),
                      strand_.wrap(
                        boost::bind(&connection::handle_read, shared_from_this(),
                          boost::asio::placeholders::error,
                          boost::asio::placeholders::bytes_transferred)));
        }

    }

    // If error occurs then no new async operations are started. All shared_ptr references to the connection object will
    // disappear and the object will be destroyed automatically after this
    // handler returns. The connection class's destructor closes the socket.
}

void connection::handle_write(const boost::system::error_code& e) {
   if (!e) {
        // Initiate graceful connection closure.
        boost::system::error_code ignored_ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
                         ignored_ec);
    }

   // If error occurs then no new async operations are started. All shared_ptr references to the connection object will
   // disappear and the object will be destroyed automatically after this
   // handler returns. The connection class's destructor closes the socket.
}

} // namespace server
} // namespace http
