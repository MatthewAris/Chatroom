#include "accepter.h"
#include "receiver.h"
#include "util.h"
#include <iostream>
#include <sstream>
#include <thread>

Accepter::Accepter(Queue<std::string>& q, List<std::shared_ptr<sf::TcpSocket>>& s):
    queue_(q),
    socket_(s)
{}

void Accepter::operator()()
{
    // TODO the listener has to listen.
    sf::TcpListener listener;
    sf::Socket::Status status;
    status = listener.listen(PORT);

    std::cout << "Bound to port \n";
    while (true)
    {
        std::shared_ptr<sf::TcpSocket> socket = std::make_shared<sf::TcpSocket>();
        // TODO accept a connection on socket
        status = listener.accept(*socket);

        if (status != sf::Socket::Done)
        {
            std::cout << "Error Accepting \n";
            return;
        }

        socket_.push(socket);
        std::stringstream ss;
        ss << "Accepted a connection from: "
            << socket->getRemoteAddress()
            << ":"
            << socket->getRemotePort()
            << std::endl;
        std::cout << ss.str();
        std::shared_ptr<Receiver> receiver = std::make_shared<Receiver>(socket, queue_);
        // TODO launch a thread to receive with the receiver
        std::thread(&Receiver::recv_loop, receiver).detach();
    }
}

