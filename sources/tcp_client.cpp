include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::tcp;


int client(int m_argc, char* m_argv[])
{
    try {
        if (m_argc != 2) {
            std::cerr << "Usage^ client <host>" << std::endl;
            return 1;
        }
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);

        tcp::resolver::results_type endpoints = resolver.resolve(m_argv[1], "daytime");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        for (;;) {
            boost::array<char, 128> buf;
            boost::system::error_code error;

            size_t len = socket.read_some(boost::asio::buffer(buf), error);

            if (error == boost::asio::error::eof)
                break;
            else if (error)
                throw boost::system::system_error(error);
            std::cout.write(buf.data(), len);
        }
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
