#include <cstdint>
#include <iostream>
#include <winsock2.h>
#include <variant>
#include <string>
#include <stdexcept>
#include <numeric>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

struct ClientMode
{
	std::string address;
	uint16_t port;
};

struct ServerMode
{
	uint16_t port;
};

using ProgramMode = std::variant<ClientMode, ServerMode>;

void Run(ServerMode mode)
{
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::acceptor acceptor(io_service,
										 boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), mode.port));

	std::cout << "Listening to the port " << Port << std::endl;

	while(true)
	{
		std::cout << "Accepting" << std::endl;

		boost::asio::ip::tcp::socket socket(io_service);
		acceptor.accept(socket);

		std::cout << "Accepted" << std::endl;

		std::array<char, BufferSize> buffer;
		boost::system::error_code error;

		while(true)
		{
			std::size_t len = socket.read_some(boost::asio::buffer(buffer), error);

			if(error == boost::asio::error::eof)
				break;
			else if(error)
				throw boost::system::system_error(error);

			boost::asio::write(socket, boost::asio::buffer(buffer, len));

			std::cout << "Client disconnected" << std::endl;
		}
	}
}

ProgramMode ParseCommandLine(int argc, char* argv[])
{
	if (argc < 2 || argc > 4)
	{
		throw std::runtime_error("Invalid command line");
	}

	if (argc == 2)
	{
		unsigned long port = std::stoul(argv[1]);
		if (port < 1 || port >= std::numeric_limits<uint16_t>::max())
		{
			throw std::runtime_error("Invalid port");
		}
		return ServerMode{ .port = static_cast<uint16_t>(port) };
	}

	if (argc == 3)
	{
		unsigned long port = std::stoul(argv[2]);
		if (port < 1 || port >= std::numeric_limits<uint16_t>::max())
		{
			throw std::runtime_error("Invalid port");
		}
		return ClientMode{ .address = argv[1], .port = static_cast<uint16_t>(port) };
	}
	throw std::runtime_error("Invalid server command line parameters");
}


void Run(const ServerMode& mode)
{
	sockaddr_in serverAddr{
		.sin_family = AF_INET,
		.sin_port = htons(mode.port),
		.sin_addr = { .s_addr = INADDR_ANY },
		// The sin_port and sin_addr members are stored in network byte order.
	};

	Acceptor acceptor{ serverAddr, 5 };

	std::cout << "Listening to the port " << Port << std::endl;

	while (true)
	{
		std::cout << "Accepting" << std::endl;
		auto clientSocket = acceptor.Accept();
		std::cout << "Accepted" << std::endl;
		char buffer[BufferSize];
		for (size_t bytesRead; (bytesRead = clientSocket.Read(&buffer, sizeof(buffer))) > 0;)
		{
			clientSocket.Send(buffer, bytesRead, 0);
		}
		std::cout << "Client disconnected" << std::endl;
	}
}

void Run(const ClientMode& mode)
{

}

int main(int argc, char *argv[])
{
	try
	{
		auto mode = ParseCommandLine(argc, argv);
		std::visit([](const auto& mode) { Run(mode); }, mode);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}