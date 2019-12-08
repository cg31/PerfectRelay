
#include <cstdlib>
#include <deque>
#include <iostream>
#include <memory>
#include <thread>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

const int BUF_LENGTH = 400 * 1024;

void relay(std::shared_ptr<tcp::socket> src, std::shared_ptr<tcp::socket> dst)
{
	for (;;)
	{
		char data[BUF_LENGTH];

		boost::system::error_code error;
		size_t length = src->read_some(boost::asio::buffer(data), error);
		if (error == boost::asio::error::eof)
			break; // Connection closed cleanly by peer.
		else if (error)
			throw boost::system::system_error(error); // Some other error.

		boost::asio::write(*dst, boost::asio::buffer(data, length));
	}
}

int main(int argc, char *argv[])
{
	std::cout << "Listening on: 127.0.0.1:1090" << std::endl;
	std::cout << "Relaying to: 127.0.0.1:5201" << std::endl;

	boost::asio::io_context io_cxt;

	tcp::acceptor listener(io_cxt, tcp::endpoint(tcp::v4(), 1090));
	for (;;)
	{
		std::shared_ptr<tcp::socket> src(new tcp::socket(io_cxt));
		listener.accept(*src);

		tcp::resolver resolver(io_cxt);
		std::shared_ptr<tcp::socket> dst(new tcp::socket(io_cxt));
		boost::asio::connect(*dst, resolver.resolve(tcp::v4(), "127.0.0.1", "5201"));

		relay(src, dst);
	}

	return 0;
}
