/*
 * Duncan Horn
 *
 * SocketStreamTest.cpp
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
            void RunServerTest(const ServerFunc &serverFunc, const ClientFunc &clientFunc)
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
            void RunSingleClientServerTest(const ServerFunc &serverFunc, const ClientFunc &clientFunc)
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

            TEST_METHOD(SimpleStringInputTest)
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

            TEST_METHOD(SimpleStringOutputTest)
            {
                static const std::string str = "this is a basic string test!";
                static const std::vector<std::string> words = { "this", "is", "a", "basic", "string", "test!" };

                RunSingleClientServerTest([&](dhorn::tcp_socket &server)
                {
                    dhorn::socket_streambuf buf(&server);
                    std::iostream stream(&buf);

                    const char *pre = "";
                    for (auto &word : words)
                    {
                        stream << pre << word;
                        stream.flush();

                        pre = " ";
                    }
                },
                    [&](dhorn::tcp_socket &client)
                {
                    std::string sentence;
                    char buffer[100];

                    while (sentence.length() < str.length())
                    {
                        auto len = client.receive(buffer);
                        buffer[len] = '\0';
                        sentence += buffer;
                    }

                    Assert::IsTrue(sentence == str);
                });
            }

            TEST_METHOD(SimpleIntegerInputTest)
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

            TEST_METHOD(SimpleIntegerOutputTest)
            {
                static const std::string str = "3 7 24 72 3 4624 42 8";
                static const std::vector<int> values = { 3, 7, 24, 72, 3, 4624, 42, 8 };

                RunSingleClientServerTest([&](dhorn::tcp_socket &server)
                {
                    dhorn::socket_streambuf buf(&server);
                    std::iostream stream(&buf);

                    char *pre = "";
                    for (auto &val : values)
                    {
                        stream << pre << val;
                        stream.flush();

                        pre = " ";
                    }
                },
                    [&](dhorn::tcp_socket &client)
                {
                    std::string output;
                    char buffer[100];

                    while (output.size() < str.length())
                    {
                        auto len = client.receive(buffer);
                        buffer[len] = '\0';
                        output += buffer;
                    }

                    Assert::IsTrue(output == str);
                });
            }
        };



        // Same as the SocketStreamBufTests, only this test uses dhorn::socket_stream instead of std::iostream
        TEST_CLASS(SocketStreamTests)
        {
            template <typename ServerFunc, typename ClientFunc>
            void RunServerTest(const ServerFunc &serverFunc, const ClientFunc &clientFunc)
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
            void RunSingleClientServerTest(const ServerFunc &serverFunc, const ClientFunc &clientFunc)
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

            TEST_METHOD(SimpleStringInputTest)
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
                    dhorn::socket_stream stream(&client);

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

            TEST_METHOD(SimpleStringOutputTest)
            {
                static const std::string str = "this is a basic string test!";
                static const std::vector<std::string> words = { "this", "is", "a", "basic", "string", "test!" };

                RunSingleClientServerTest([&](dhorn::tcp_socket &server)
                {
                    dhorn::socket_stream stream(&server);

                    const char *pre = "";
                    for (auto &word : words)
                    {
                        stream << pre << word;
                        stream.flush();

                        pre = " ";
                    }
                },
                    [&](dhorn::tcp_socket &client)
                {
                    std::string sentence;
                    char buffer[100];

                    while (sentence.length() < str.length())
                    {
                        auto len = client.receive(buffer);
                        buffer[len] = '\0';
                        sentence += buffer;
                    }

                    Assert::IsTrue(sentence == str);
                });
            }

            TEST_METHOD(SimpleIntegerInputTest)
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
                    dhorn::socket_stream stream(&client);

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

            TEST_METHOD(SimpleIntegerOutputTest)
            {
                static const std::string str = "3 7 24 72 3 4624 42 8";
                static const std::vector<int> values = { 3, 7, 24, 72, 3, 4624, 42, 8 };

                RunSingleClientServerTest([&](dhorn::tcp_socket &server)
                {
                    dhorn::socket_stream stream(&server);

                    char *pre = "";
                    for (auto &val : values)
                    {
                        stream << pre << val;
                        stream.flush();

                        pre = " ";
                    }
                },
                    [&](dhorn::tcp_socket &client)
                {
                    std::string output;
                    char buffer[100];

                    while (output.size() < str.length())
                    {
                        auto len = client.receive(buffer);
                        buffer[len] = '\0';
                        output += buffer;
                    }

                    Assert::IsTrue(output == str);
                });
            }
        };
    }
}
