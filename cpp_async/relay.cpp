
#include <cstdlib>
#include <deque>
#include <iostream>
#include <memory>
#include <thread>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

const int BUF_LENGTH = 400 * 1024;


class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(boost::asio::io_context &io_cxt, tcp::socket socket)
        : m_cxt(io_cxt), m_src(std::move(socket)), m_dst(tcp::socket(m_cxt))
    {
        tcp::resolver resolver(m_cxt);
        boost::asio::connect(m_dst, resolver.resolve(tcp::v4(), "127.0.0.1", "5201"));
    }

    void start()
    {
        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        m_src.async_read_some(boost::asio::buffer(m_buf, BUF_LENGTH), [this, self](boost::system::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                do_write(length);
            }
        });
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        boost::asio::async_write(m_dst, boost::asio::buffer(m_buf, length), [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                do_read();
            }
        });
    }

    boost::asio::io_context &m_cxt;
    tcp::socket m_src;
    tcp::socket m_dst;

    char m_buf[BUF_LENGTH];
};

class Relay
{
public:
    Relay(boost::asio::io_context &io_cxt)
        : m_cxt(io_cxt), m_listener(io_cxt, tcp::endpoint(tcp::v4(), 1090))
    {
        do_accept();
    }

private:
    void do_accept()
    {
        m_listener.async_accept([this](boost::system::error_code ec, tcp::socket socket)
        {
            if (!ec)
            {
                std::make_shared<Session>(m_cxt, std::move(socket))->start();
            }

            do_accept();
        });
    }

    boost::asio::io_context &m_cxt;
    tcp::acceptor m_listener;
};

int main(int argc, char *argv[])
{
    std::cout << "Listening on: 127.0.0.1:1090" << std::endl;
    std::cout << "Relaying to: 127.0.0.1:5201" << std::endl;

    boost::asio::io_context io_cxt;

    Relay relay(io_cxt);

    io_cxt.run();

    return 0;
}
