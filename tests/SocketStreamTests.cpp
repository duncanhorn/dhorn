/*
 * SocketStreamTest.cpp
 *
 * Duncan Horn
 *
 * Tests for the dhorn socket_stream classes, structures, functions, etc.
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include <string>
#include <thread>
#include <vector>

#include "dhorn/socket_stream.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(SocketStreamBufTests)
        {
            template <typename ServerFunc, typename ClientFunc>
            void RunServerTest(_In_ const ServerFunc &serverFunc, _In_ const ClientFunc &clientFunc)
            {
                static const uint16_t port = 1337;

                // Create the server first to avoid any race conditions
                dhorn::server_socket server;
                dhorn::socket_address serverAddr(dhorn::ipv4_address(dhorn::any_address), port);
                server.bind(serverAddr);
                server.listen(5);

                // Now, run the client function in a different thread
                std::thread clientThread([&]()
                {
                    clientFunc(port);
                });

                // There's no reason to run both in different threads, so just run the server function here
                serverFunc(server);

                // Now stop the server and wait for the client to complete
                server.close();
                clientThread.join();
            }

            template <typename ServerFunc, typename ClientFunc>
            void RunSingleClientServerTest(_In_ const ServerFunc &serverFunc, _In_ const ClientFunc &clientFunc)
            {
                RunServerTest([&](dhorn::server_socket &server)
                {
                    // For the server, we just need to accept one connection
                    dhorn::socket_address addr;
                    auto clientSocket = server.accept(addr);

                    serverFunc(clientSocket);

                    clientSocket.shutdown(dhorn::shutdown_options::send);
                    clientSocket.close();
                },
                    [&](uint16_t port)
                {
                    dhorn::socket_address addr(dhorn::ipv4_address(dhorn::local_host), port);
                    dhorn::tcp_socket sock;
                    sock.connect(addr);

                    clientFunc(sock);

                    sock.shutdown(dhorn::shutdown_options::send);
                    sock.close();
                });
            }

            TEST_METHOD(SimpleStringOutputTest)
            {
                static const std::string str = "this is a basic string test!";
                static const std::vector<std::string> words = { "this", "is", "a", "basic", "string", "test!" };

                RunSingleClientServerTest([&](dhorn::tcp_socket &server)
                {
                    // Since this is a simple/basic test, we're not going to bother with sending partial strings yet
                    server.send(std::begin(str), std::end(str));
                },
                    [&](dhorn::tcp_socket &client)
                {
                    dhorn::socket_streambuf buf(&client);
                    std::iostream stream(&buf);

                    std::vector<std::string> data;
                    std::string word;
                    while (stream >> word)
                    {
                        data.push_back(word);
                    }

                    Assert::AreEqual(data.size(), words.size());
                    for (size_t i = 0; i < data.size(); ++i)
                    {
                        Assert::IsTrue(data[i] == words[i]);
                    }
                });
            }

            TEST_METHOD(SimpleIntegerTest)
            {
                static const std::string str = "3 7 24 72 3 4624 42 8";
                static const std::vector<int> values = { 3, 7, 24, 72, 3, 4624, 42, 8 };

                RunSingleClientServerTest([&](dhorn::tcp_socket &server)
                {
                    // Since this is a simple/basic test, we're not going to bother with sending partial strings yet
                    server.send(std::begin(str), std::end(str));
                },
                    [&](dhorn::tcp_socket &client)
                {
                    dhorn::socket_streambuf buf(&client);
                    std::iostream stream(&buf);

                    std::vector<int> data;
                    int value;
                    while (stream >> value)
                    {
                        data.push_back(value);
                    }

                    Assert::AreEqual(data.size(), values.size());
                    for (size_t i = 0; i < data.size(); ++i)
                    {
                        Assert::IsTrue(data[i] == values[i]);
                    }
                });
            }
        };
    }
}
