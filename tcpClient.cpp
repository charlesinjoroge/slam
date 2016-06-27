/**
    C++ client example using sockets
*/
#include <iostream>    //cout
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <string>  //string
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <netdb.h> //hostent
#include "json/src/json.hpp"
 
using namespace std;
using json = nlohmann::json;
 
/**
    TCP Client class
*/
class tcp_client {
private:
    int sock;
    std::string address;
    int port;
    struct sockaddr_in server;
     
public:
    tcp_client();
    bool conn(string, int);
    bool send_data(string data);
    string receive(int);
};
 
tcp_client::tcp_client() {
    sock = -1;
    port = 0;
    address = "";
}
 
/**
    Connect to a host on a certain port number
*/
bool tcp_client::conn(string address , int port) {
    //create socket if it is not already created
    if(sock == -1)
    {
        //Create socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
        {
            perror("Could not create socket");
        }
         
        cout<<"Socket created\n";
    }
    else    {   /* OK , nothing */  }
     
    //setup address structure
    if(inet_addr(address.c_str()) == -1)
    {
        struct hostent *he;
        struct in_addr **addr_list;
         
        //resolve the hostname, its not an ip address
        if ( (he = gethostbyname( address.c_str() ) ) == NULL)
        {
            //gethostbyname failed
            herror("gethostbyname");
            cout<<"Failed to resolve hostname\n";
             
            return false;
        }
         
        //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
        addr_list = (struct in_addr **) he->h_addr_list;
 
        for(int i = 0; addr_list[i] != NULL; i++)
        {
            //strcpy(ip , inet_ntoa(*addr_list[i]) );
            server.sin_addr = *addr_list[i];
             
            cout<<address<<" resolved to "<<inet_ntoa(*addr_list[i])<<endl;
             
            break;
        }
    }
     
    //plain ip address
    else
    {
        server.sin_addr.s_addr = inet_addr( address.c_str() );
    }
     
    server.sin_family = AF_INET;
    server.sin_port = htons( port );
     
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    cout<<"Connected\n";
    return true;
}
 
/**
    Send data to the connected host
*/
bool tcp_client::send_data(string data) {
    //Send some data
    if( send(sock , data.c_str() , strlen( data.c_str() ) , 0) < 0)
    {
        perror("Send failed : ");
        return false;
    }
    cout<<"Data send\n";
     
    return true;
}
 
/**
    Receive data from the connected host
*/
string tcp_client::receive(int size) {

    char buffer[size]; // the size should grow
    string reply;

    if(recv(sock , buffer, sizeof(buffer) , 0) < 0){
    	perror("recv error");
    }

    reply = buffer;

    return reply; // return nothing if there was an error
}

string globalBuffer; // the global buffer

string getJson(tcp_client client){

    string final; // the string to return

    globalBuffer.append(client.receive(1020)); // receive from the client

    while(globalBuffer.length() != 0){
        for(int i = 0; i < globalBuffer.length(); i++){
            final += globalBuffer[i];

            // cout << final[final.length() - 1] + " final";
            // cout << (final[final.length() - 1] == '}');
            // cout << globalBuffer[i + 1] + " global";
            // cout <<  (globalBuffer[i + 1] == '{');

            if(final[final.length() - 1] == '}' && globalBuffer[i + 1] == '{'){
                globalBuffer = globalBuffer.substr(i , globalBuffer.length()); // chop the string
                return final; // return the finished chunk
            }
        }
        globalBuffer = client.receive(1020); // if nothing was found then move on
    }


    return final;

}

 
int main(int argc , char *argv[])
{
    tcp_client odomClient; // odom client 
    tcp_client baseClient; // base scan client
     
    //connect to host
    odomClient.conn("fetch18" , 9090);
   	baseClient.conn("fetch18" , 9090);

    json odom; // make the json object
    odom["op"] = "subscribe";
    odom["topic"] = "/odom";

    json base;
    base["op"] = "subscribe";
    base["topic"] = "/base_scan";
 
    std::string odomDump = odom.dump();
    std::string baseDump = base.dump();

    std::cout << "Sending odom json ..." ;
   	odomClient.send_data(odomDump);

    std::cout << "Sending base json ..." ;
    baseClient.send_data(baseDump);

 // while(1){
 	
 	// cout<<"-----------Odom-------------\n\n";
  //   //receive and echo reply
  //   cout<<odomClient.receive(1020); // number of bits needed for odom readings
  //   cout<<"\n\n----------------------------\n\n";

  //   cout<<"--------------Base------------\n\n";
    //receive and echo reply
     cout << getJson(baseClient);

//     cout<<"\n\n-----------Base part 2-------------\n\n";
//     cout<<baseClient.receive(1020);
// cout<<"\n\n-----------Base part 2-------------\n\n";
//     cout<<baseClient.receive(1020);
//     cout<<"\n\n----------------------------\n\n";
 	
// }    

    return 0;
}