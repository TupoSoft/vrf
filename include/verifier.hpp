#pragma once

#include "resolver.hpp"
#include "vrf_data.hpp"

#include "boost/asio.hpp"

namespace asio = boost::asio;
namespace dns = kyrylokupin::asio::dns;

namespace tuposoft::tupovrf {
    class verifier {
    public:
        explicit verifier(const asio::any_io_executor &executor) : resolv_(executor), socket_(executor) {}

        auto verify(std::string) -> asio::awaitable<vrf_data>;

    private:
        dns::resolver resolv_;
        asio::ip::tcp::socket socket_;

        static constexpr auto SMTP_PORT = 25;
    };
} // namespace tuposoft::vrf
