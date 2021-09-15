#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

#include "root_certificates.hpp"
#include "json.hpp"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

// Sends a WebSocket message and prints the response

//int proposal() {
//
//}

int makeStakeBeast()
{
    try
    {
        std::vector<std::string> vectorWithData = {"", "", "", ""};
        vectorWithData[0] = "websocket-client-sync-ssl";
        vectorWithData[1] = "ws.binaryws.com";
        vectorWithData[2] = "443";
        vectorWithData[3] = "{\n"
                            "  \"authorize\": \"jwsoAq9ZSZcHHVK\"\n"
                            "}";
        auto target = "/websockets/v3?app_id=1089&l";

        // Check command line arguments.
        if(vectorWithData.size() != 4)
        {
            std::cerr <<
                      "Usage: websocket-client-sync-ssl <host> <port> <text>\n" <<
                      "Example:\n" <<
                      "    websocket-client-sync-ssl echo.websocket.org 443 \"Hello, world!\"\n";
            return EXIT_FAILURE;
        }
        std::string host = vectorWithData[1];
        auto const  port = vectorWithData[2];
        auto const  text = vectorWithData[3];

        // The io_context is required for all I/O
        net::io_context ioc;

        // The SSL context is required, and holds certificates
        ssl::context ctx{ssl::context::tlsv12_client};

        // This holds the root certificate used for verification
        load_root_certificates(ctx);

        // These objects perform our I/O
        tcp::resolver resolver{ioc};
        websocket::stream<beast::ssl_stream<tcp::socket>> ws{ioc, ctx};

        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        // Make the connection on the IP address we get from a lookup
        auto ep = net::connect(get_lowest_layer(ws), results);

        // Set SNI Hostname (many hosts need this to handshake successfully)
        if(! SSL_set_tlsext_host_name(ws.next_layer().native_handle(), host.c_str()))
            throw beast::system_error(
                    beast::error_code(
                            static_cast<int>(::ERR_get_error()),
                            net::error::get_ssl_category()),
                    "Failed to set SNI Hostname");

        // Update the host_ string. This will provide the value of the
        // Host HTTP header during the WebSocket handshake.
        // See https://tools.ietf.org/html/rfc7230#section-5.4
        host += ':' + std::to_string(ep.port());

        // Perform the SSL handshake
        ws.next_layer().handshake(ssl::stream_base::client);

        // Set a decorator to change the User-Agent of the handshake
        ws.set_option(websocket::stream_base::decorator(
                [](websocket::request_type& req)
                {
                    req.set(http::field::user_agent,
                            std::string(BOOST_BEAST_VERSION_STRING) +
                            " websocket-client-coro");
                }));

        // Perform the websocket handshake
        ws.handshake(host, target);

        // Send the message
        ws.write(net::buffer(std::string(text)));

        // This buffer will hold the incoming message
        beast::flat_buffer buffer;

        // Read a message into our buffer
//        while (ws.is_message_done()) {
//            std::cout << "ws.is_message_done() != true" << std::endl;
//            buffer.prepare(10000);
//            std::cout << "buffer.size() before read" << buffer.size() << std::endl;
//            boost::system::error_code ec;
//            ws.read(buffer, ec);
////            ws.read_some(buffer, 10000, ec);
//            }
//            std::cout << "ec = " << ec << std::endl;
//            std::cout << "buffer.size() after read" << buffer.size() << std::endl;
//            std::cout << "data from buffer = "
//                         "" << boost::beast::buffers_to_string(buffer.data()) << std::endl;
//        }


        boost::system::error_code ec;
        ws.read(buffer);
        std::cout << "buffer after authorize = "
                  << boost::beast::buffers_to_string(buffer.data()) << std::endl;
        std::cout << "Clearing buffer after authorize" << std::endl;
        buffer.clear();


        auto proposalText = "{"
                            "  \"proposal\": 1,"
                            "  \"contract_type\": \"CALL\","
                            "  \"duration\": 5,"
                            "  \"duration_unit\": \"m\","
                            "  \"basis\": \"stake\","
                            "  \"amount\": 100,"
                            "  \"currency\": \"USD\","
                            "  \"symbol\": \"frxUSDJPY\""
                            "}";

        ws.write(net::buffer(std::string(proposalText)));
        ws.read(buffer, ec);

        auto bufferResponse = boost::beast::buffers_to_string(buffer.data());


        std::cout << "buffer after proposal = "
                  << bufferResponse << std::endl;

        nlohmann::json jsonObject;
        jsonObject = nlohmann::json::parse(bufferResponse);
        auto id = jsonObject["proposal"]["id"];
        std::cout << "id = " << id << std::endl;


        std::cout << "Clearing buffer after proposal" << std::endl;
        buffer.clear();

//        auto buyText = "{"
//                          "\"buy\":1,"
//                          "\"price\": 1000, "
//                          "\"parameters\":"
//                          " {"
//                          "     \"contract_type\":\"CALL\","
//                          "     \"symbol\": \"frxUSDJPY\","
//                          "     \"currency\": \"USD\","
//                          "     \"duration\": 5,"
//                          "     \"duration_unit\": \"m\","
//                          "     \"basis\": \"stake\","
//                          "     \"amount\": 1"
//                          " }"
//                          "}";

        auto buyText = "{"
                       "\"buy\": \"" + std::string(id) + "\","
                                                         "\"price\": 1000"
                                                         "}";

        std::cout << "buyText = " << buyText << std::endl;

        ws.write(net::buffer(std::string(buyText)));
        ws.read(buffer, ec);

        std::cout << "\nbuffer after buy = "
                  << boost::beast::buffers_to_string(buffer.data()) << "\n" << std::endl;

        // Close the WebSocket connection
        ws.close(websocket::close_code::normal, ec);

        // If we get here then the connection is closed gracefully

        // The make_printable() function helps print a ConstBufferSequence
        std::cout << beast::make_printable(buffer.data()) << std::endl;
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}