#ifndef HEADER_HPP
#define HEADER_HPP

#include <string>

namespace http {
namespace server {

struct header {
    std::string name;
    std::string value;
};

} // namespace server
} // namespace http

#endif // HEADER_HPP
