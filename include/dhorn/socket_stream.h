/*
 * socket_stream.h
 *
 * Duncan Horn
 *
 * Implements a std::streambuf class that is powered by a dhorn::tcp_socket for use with std::iostream, etc. There are
 * also simple wrappers that implement std::iostream, etc. for simpler use. E.g. without them, you'd have to do
 * something like:
 *
 *      dhorn::socket_streambuf buf(&this->_socket);
 *      std::iostream stream(&buf);
 *
 * But with them, this simplifies slightly to something easier on the eyes:
 *
 *      dhorn::socket_stream stream(&this->_socket);
 *
 * By default, the SocketStorageType template argument type is dhorn::tcp_socket *. If the client wishes to use
 * something different (e.g. std::shared_ptr<dhorn::tcp_socket>), that can be specified in its place.
 */

#include <memory>
#include <streambuf>

#include "sockets.h"

namespace dhorn
{
#pragma region basic_socket_streambuf

    template <
        typename CharT,
        typename CharTraits = std::char_traits<CharT>,
        size_t BufferSize = 2048,
        typename SocketStorageType = dhorn::tcp_socket *>
    class basic_socket_streambuf final :
        public std::basic_streambuf<CharT, CharTraits>
    {
    public:
        basic_socket_streambuf(_In_ SocketStorageType socket) :
            _socket(socket),
            _buffer(new CharT[BufferSize])
        {
        }


        /*
         * basic_streambuf functions
         */
        virtual int underflow(void)
        {
            if (this->gptr() == this->egptr())
            {
                // We're out of data; try and read more from the socket
                this->_len = this->_socket->receive(this->_buffer.get(), BufferSize * sizeof(CharT));
                this->setg(this->_buffer.get(), this->_buffer.get(), this->_buffer.get() + this->_len);
            }

            return (this->gptr() == this->egptr()) ?
                traits_type::eof() :
                traits_type::to_int_type(*this->gptr());
        }



    private:

        SocketStorageType _socket;
        std::unique_ptr<CharT[]> _buffer;
        size_t _len;
    };



    /*
     * Type definitions
     */
    using socket_streambuf = basic_socket_streambuf<char>;
    using wsocket_streambuf = basic_socket_streambuf<wchar_t>;

#pragma endregion
}
