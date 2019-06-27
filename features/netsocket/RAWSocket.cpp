/*
 * $ Copyright Cypress Semiconductor Apache2 $
*/

#include "RAWSocket.h"
#include "Timer.h"
#include "mbed_assert.h"

RAWSocket::RAWSocket()
{
    _socket_stats.stats_update_proto(this, NSAPI_RAW);
}

RAWSocket::~RAWSocket()
{
}

nsapi_protocol_t RAWSocket::get_proto()
{
    return NSAPI_RAW;
}

nsapi_error_t RAWSocket::connect(const SocketAddress &address)
{
    _remote_peer = address;
    _socket_stats.stats_update_peer(this, _remote_peer);
    _socket_stats.stats_update_socket_state(this, SOCK_CONNECTED);
    return NSAPI_ERROR_OK;
}

nsapi_size_or_error_t RAWSocket::sendto(const char *host, uint16_t port, const void *data, nsapi_size_t size)
{
    SocketAddress address;
    nsapi_size_or_error_t err;

    if (!strcmp(_interface_name, "")) {
        err = _stack->gethostbyname(host, &address);
    } else {
        err = _stack->gethostbyname(host, &address, NSAPI_UNSPEC, _interface_name);
    }

    if (err) {
        return NSAPI_ERROR_DNS_FAILURE;
    }

    address.set_port(port);

    // sendto is thread safe
    return sendto(address, data, size);
}

nsapi_size_or_error_t RAWSocket::sendto(const SocketAddress &address, const void *data, nsapi_size_t size)
{
    _lock.lock();
    nsapi_size_or_error_t ret;

    _writers++;
    if (_socket) {
        _socket_stats.stats_update_socket_state(this, SOCK_OPEN);
        _socket_stats.stats_update_peer(this, address);
    }
    while (true) {
        if (!_socket) {
            ret = NSAPI_ERROR_NO_SOCKET;
            break;
        }

        core_util_atomic_flag_clear(&_pending);
        nsapi_size_or_error_t sent = _stack->socket_sendto(_socket, address, data, size);
        if ((0 == _timeout) || (NSAPI_ERROR_WOULD_BLOCK != sent)) {
            _socket_stats.stats_update_sent_bytes(this, sent);
            ret = sent;
            break;
        } else {
            uint32_t flag;

            // Release lock before blocking so other threads
            // accessing this object aren't blocked
            _lock.unlock();
            flag = _event_flag.wait_any(WRITE_FLAG, _timeout);
            _lock.lock();

            if (flag & osFlagsError) {
                // Timeout break
                ret = NSAPI_ERROR_WOULD_BLOCK;
                break;
            }
        }
    }

    _writers--;
    if (!_socket || !_writers) {
        _event_flag.set(FINISHED_FLAG);
    }
    _lock.unlock();
    return ret;
}

nsapi_size_or_error_t RAWSocket::send(const void *data, nsapi_size_t size)
{
    if (!_remote_peer) {
        return NSAPI_ERROR_NO_ADDRESS;
    }
    return sendto(_remote_peer, data, size);
}

nsapi_size_or_error_t RAWSocket::recvfrom(SocketAddress *address, void *buffer, nsapi_size_t size)
{
    _lock.lock();
    nsapi_size_or_error_t ret;
    SocketAddress ignored;

    if (!address) {
        address = &ignored;
    }

    _readers++;

    if (_socket) {
        _socket_stats.stats_update_socket_state(this, SOCK_OPEN);
    }
    while (true) {
        if (!_socket) {
            ret = NSAPI_ERROR_NO_SOCKET;
            break;
        }

        core_util_atomic_flag_clear(&_pending);
        nsapi_size_or_error_t recv = _stack->socket_recvfrom(_socket, address, buffer, size);

        // Filter incomming packets using connected peer address
        if (recv >= 0 && _remote_peer && _remote_peer != *address) {
            continue;
        }

        _socket_stats.stats_update_peer(this, _remote_peer);
        // Non-blocking sockets always return. Blocking only returns when success or errors other than WOULD_BLOCK
        if ((0 == _timeout) || (NSAPI_ERROR_WOULD_BLOCK != recv)) {
            ret = recv;
            _socket_stats.stats_update_recv_bytes(this, recv);
            break;
        } else {
            uint32_t flag;

            // Release lock before blocking so other threads
            // accessing this object aren't blocked
            _lock.unlock();
            flag = _event_flag.wait_any(READ_FLAG, _timeout);
            _lock.lock();

            if (flag & osFlagsError) {
                // Timeout break
                ret = NSAPI_ERROR_WOULD_BLOCK;
                break;
            }
        }
    }

    _readers--;
    if (!_socket || !_readers) {
        _event_flag.set(FINISHED_FLAG);
    }

    _lock.unlock();
    return ret;
}

nsapi_size_or_error_t RAWSocket::recv(void *buffer, nsapi_size_t size)
{
    return recvfrom(NULL, buffer, size);
}

Socket *RAWSocket::accept(nsapi_error_t *error)
{
    if (error) {
        *error = NSAPI_ERROR_UNSUPPORTED;
    }
    return NULL;
}

nsapi_error_t RAWSocket::listen(int)
{
    return NSAPI_ERROR_UNSUPPORTED;
}
