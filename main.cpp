#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <Loop.h>

#include <cmath>
#include <iostream>
#include <thread>

void Server()
{
    sf::TcpListener listener;
    sf::Socket::Status status;
    status = listener.listen(4300);
    if (status != sf::Socket::Done)
    {
        std::cout << "Error Listening \n";
        return;
    }

    sf::TcpSocket socket;
    status = listener.accept(socket);
    if (status != sf::Socket::Done)
    {
        std::cout << "Error Accepting \n";
        return;
    }

    sf::Packet packet;
    status = socket.receive(packet);
    if (status != sf::Socket::Done)
    {
        std::cout << "Error Receiving \n";
        return;
    }
    std::string message;
    packet >> message;
    std::cout << message;
    packet.clear();
    packet << "message received";
    status = socket.send(packet);
    if (status != sf::Socket::Done)
    {
        std::cout << "Error Sending \n";
        return;
    }
}

void Client()
{
    sf::TcpSocket socket;
    sf::IpAddress address("localhost");
    sf::Socket::Status status;
    status = socket.connect(address, 4300);
    if (status != sf::Socket::Done)
    {
        std::cout << "Error Connecting \n";
        return;
    }
    sf::Packet packet;
    packet << "Hi!";
    status = socket.send(packet);
    if (status != sf::Socket::Done)
    {
        std::cout << "Error Sending \n";
        return;
    }
    packet.clear();
    status = socket.receive(packet);
    if (status != sf::Socket::Done)
    {
        std::cout << "Error Receiving \n";
        return;
    }
    std::string message;
    packet >> message;
    std::cout << message;
}

int main()
{
    std::thread serverThread(&Server);
    //std::this_thread::sleep_for(std::chrono::microseconds(10));
    Client();
    serverThread.join();
    return 0;
}


