#include "accepter.h"
#include "queue.h"
#include "list.h"
#include "receiver.h"
#include "util.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>

void UdpServer()
{
    sf::UdpSocket socket;
    unsigned int local_port;
    std::cout << "Please type in the server port: ";
    std::cin >> local_port;

    if (socket.bind(local_port) != sf::Socket::Done)
    {
        std::cerr << "Binding error" << std::endl;
        return;
    }
    sf::IpAddress sender;
    unsigned short remote_port;
    while(true)
    {
        sf::Packet packet;
        sf::Socket::Status status = socket.receive(packet, sender, remote_port);
        std::string s;
        packet >> s;
        std::cout << "Server received: " << s << std::endl;
        packet.clear();
        packet << "Broadcast";
        // in the game you might want to send the seed for rand, so that all
        // the clients will work with the same random colours.
        socket.send(packet, sender, remote_port);
    }
}

int Server()
{
    Queue<std::string> queue;
    List<std::shared_ptr<sf::TcpSocket>> sockets;
    Accepter accepter(queue, sockets);
    // TODO launch an accepter thread.
    std::thread accepterThread(accepter);
    accepterThread.join();
    while(1)
    {
        std::string s = queue.pop();
        std::cout << "Main read: \"" << s << "\"\n";
        // TODO send to all in sockets. Be careful to synchronise.
    }
    return 0;
}

int UdpClient()
{
    unsigned short remote_port;
    std::cout << "Please type in the remote port: ";
    std::cin >> remote_port;
    sf::UdpSocket socket;

    // Send
    sf::Packet packet;
    packet << "Broadcast Message";
    sf::Socket::Status status = socket.send(packet, sf::IpAddress::Broadcast, remote_port);
    if (status != sf::Socket::Done)
    {
        std::cout << "Broadcast Failed\n";
        return 0;
    }

    socket.setBlocking(false);
    sf::Clock clock;
    while (clock.getElapsedTime().asMilliseconds() < 1000);

    // Receive
    sf::IpAddress sender;
    unsigned short port;
    packet.clear();
    status = socket.receive(packet, sender, port);

    // If we don't get the message from the server
    if (status != sf::Socket::Done)
    {
        std::cout << "I am a server now\n";
        std::cout << "Let the main thread die here but not in the game\n";
        std::thread(UdpServer).join();  // join here because this will not be used
                                        // for sending more messages here.
        // In the game, we want to connect with TCP an start playing the game.
    }

    // If we did get a message from the server.
    std::string s;
    packet >> s;
    std::cout << s << std::endl;
    std::cout << "The addess of the server is: " << sender << std::endl;
    return 0;
}

int Client()
{
    // 1. set up udp socket
    // 2. send bcast message
    // 3. set socket to non blocking
    // 4. receive msg
    // if no answer then spawn server
    // else connect

    std::shared_ptr<sf::TcpSocket> socket = std::make_shared<sf::TcpSocket>();
    // auto connect = [&] { return socket->connect(sf::IpAddress::getLocalAddress(), PORT); };
    //net_run(connect, "connect");
    //sf::IpAddress::getLocalAddress()
    sf::Socket::Status status = socket->connect(sf::IpAddress("152.105.67.169"), PORT);
    if (status != sf::Socket::Done) {
        return 1;
    }
    std::cout << "Connected\n";
    Queue<std::string> queue;

    // TODO launch a receiver thread to receive messages from the server.
    Receiver receiver(socket, queue);
    std::thread receiverThread(&Receiver::recv_loop, receiver);

    std::string s = "";
    while (1)
    {
        std::getline(std::cin, s);
        std::cout << "Sending: \"" << s << "\"" << std::endl;
        status = socket->send(s.c_str(), s.size());
        // check if send is successful
        //if (status != sf::Socket::Done)
        //{
        //    std::cout << "Error Sending \n";
        //    return 0;
        //}
        //queue.push(s);
        //break;
        // TODO send messages to the server
    }

    receiverThread.join();
    return 0;
}

int main()
{
   std::string choice = "";
    while(!(choice == "CLIENT" || choice == "SERVER")){
        std::cout << "Client or Server?" << std::endl;
        std::cin >> choice;
        std::transform(choice.begin(), choice.end(), choice.begin(), ::toupper);
    }
    if (choice == "CLIENT"){
        std::thread clientThread (UdpClient);
        clientThread.join();
    }
    if (choice == "SERVER"){
        std::thread serverThread(UdpServer);
        serverThread.join();
    }
}


