#include <boost/thread.hpp>
#include <boost/bind.hpp>
//#include <boost/shared_ptr.hpp>
#include <vector>

#include "../includes/server.hpp"

namespace http {
namespace server {

server::server(const std::string& address, const std::string& port,
               const std::string& doc_root, size_t thread_pool_size)
    : thread_pool_size_(thread_pool_size),
      signals_(io_service_),
      acceptor_(io_service_),
//      connection_manager_(),
//      socket_(io_service_),
      new_connection_(),
      request_handler_(doc_root)
{
    // Register to handle the signals that indicate when the server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through asio.
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
  #if defined(SIGQUIT)
    signals_.add(SIGQUIT);
  #endif

    signals_.async_wait(boost::bind(&server::handle_stop, this));

    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    boost::asio::ip::tcp::resolver resolver(io_service_);
    boost::asio::ip::tcp::resolver::query query(address, port);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    start_accept();
}

void server::run() {
    // Create a pool of threads to run all of the io_services.
    std::vector<boost::shared_ptr<boost::thread>> threads;
    for (size_t i = 0; i < thread_pool_size_; ++i) {
        boost::shared_ptr<boost::thread> thread(new boost::thread(
                                                    boost::bind(&boost::asio::io_service::run, &io_service_)));
        threads.push_back(thread);
    }

    // Wait for all threads in the pool to exit.
    for (size_t i = 0; i < threads.size(); ++i) {
        threads[i]->join();
    }
}

void server::start_accept() {
//    acceptor_.async_accept(socket_,
//                           [this](boost::system::error_code ec) {
//       // Check whether the server was stopped by a signal before this
//       // completion handler had a chance to run.
//        if (!acceptor_.is_open()) {
//            return;
//        }

//        if (!ec) {
//            connection_manager_.start(std::make_shared<connection>(
//                                          std::move(socket_), connection_manager_, request_handler_));
//        }

//        do_accept();
//    });
    new_connection_.reset(new connection(io_service_, request_handler_));
    acceptor_.async_accept(new_connection_->socket(),
                           boost::bind(&server::handle_accept, this,
                                       boost::asio::placeholders::error));
}

void server::handle_accept(const boost::system::error_code& e) {
    if (!e) {
        new_connection_->start();
    }

    start_accept();
}

void server::handle_stop() {
//    signals_.async_wait(
//                [this](boost::system::error_code /*ec*/, int /*signo*/) {
//        // The server is stopped by cancelling all outstanding asynchronous
//        // operations. Once all operations have finished the io_service::run()
//        // call will exit.
//        acceptor_.close();
//        connection_manager_.stop_all();
//    });
    io_service_.stop();
}

} //namespace server
} //namespace http
