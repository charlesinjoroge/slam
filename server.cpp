#include "websocketpp/config/asio_no_tls.hpp"

#include "websocketpp/server.hpp"

#include <iostream>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

// Define a callback to handle incoming messages
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;

    // check for a special command to instruct the server to stop listening so
    // it can be cleanly exited.
    if (msg->get_payload() == "stop-listening") {
        s->stop_listening();
        return;
    }

    try {
        // create the other socket 
        // fill in the address of the server 
        // resolve the server address 
        // connect to the remote server 
        // read and write to the speervers

        int s0 = socket(AF_INET, SOCK_STREAM, 0);
        if(s0 < 0){
            perror("Cannot create a socket");
            exit(1);
        }

        struct sockaddr_in peeraddr;
        int peeraddr_len;
        memset(&peeraddr, 0 , sizeof(peeraddr));
        char* peerHost = "localhost"; // this will change to fetch
        
        struct hostent *host = gethostbyname(peerHost);
        if(host == NULL){
            perror("Cannot define host address");
            exit(1);
        }

        peeraddr.sin_family = AF_INET;
        short peerPort = 9090;
        peeraddr.sin_port = htons(peerPort);

        int res = connect(s0, (struct sockaddr*) &peeraddr, sizeof(peeraddr));
        if(res < 0){
            perror("Cannont connect");
            exit(1);
        }
        
        printf("Connected. Sending a server message.");

        // write the message for a certain amount of time 
        // read the response for a certain amount of time

        s->send(hdl, msg->get_payload(), msg->get_opcode());
    } catch (const websocketpp::lib::error_code& e) {
        std::cout << "Echo failed because: " << e
                  << "(" << e.message() << ")" << std::endl;
    }
}

int main() {
    // Create a server endpoint
    server echo_server;

    try {
        // Set logging settings
        echo_server.set_access_channels(websocketpp::log::alevel::all);
        echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        echo_server.init_asio();

        // Register our message handler
        echo_server.set_message_handler(bind(&on_message,&echo_server,::_1,::_2));

        // Listen on port 9002
        echo_server.listen(8080);

        // Start the server accept loop
        echo_server.start_accept();

        // Start the ASIO io_service run loop
        echo_server.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "other exception" << std::endl;
    }
}
