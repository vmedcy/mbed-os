/*
 * $ Copyright Cypress Semiconductor Apache2 $
*/


#ifndef RAWSOCKET_H
#define RAWSOCKET_H

#include "netsocket/InternetSocket.h"
#include "netsocket/NetworkStack.h"
#include "netsocket/NetworkInterface.h"
#include "rtos/EventFlags.h"


/** RAW socket implementation.
 */
class RAWSocket : public InternetSocket {
public:
    /** Create an uninitialized socket.
     *
     *  @note Must call open to initialize the socket on a network stack.
     */
    RAWSocket();

    /** Create and open a socket on the network stack of the given
     *  network interface.
     *
     *  @tparam S    Type of the Network stack.
     *  @param stack Network stack as target for socket.
     *  @deprecated since mbed-os-5.11
     */
    template <typename S>
    MBED_DEPRECATED_SINCE("mbed-os-5.11",
                          "The RAWSocket(S *stack) constructor is deprecated"
                          "It discards the open() call return value."
                          "Use another constructor and call open() explicitly, instead.")
    RAWSocket(S *stack)
    {
        open(stack);
    }

    /** Destroy a socket.
     *
     *  @note Closes socket if the socket is still open.
     */
    virtual ~RAWSocket();

    /** Send data to the specified host and port.
     *
     *  By default, sendto blocks until data is sent. If socket is set to
     *  nonblocking or times out, NSAPI_ERROR_WOULD_BLOCK is returned
     *  immediately.
     *
     *  @param host     Domain name of the remote host or a dotted notation IP address.
     *  @param port     Port of the remote host.
     *  @param data     Buffer of data to send to the host.
     *  @param size     Size of the buffer in bytes.
     *  @return         Number of sent bytes on success, negative error
     *                  code on failure.
     */
    nsapi_size_or_error_t sendto(const char *host, uint16_t port,
                                 const void *data, nsapi_size_t size);

    /** Send data to the specified address.
     *
     *  By default, sendto blocks until data is sent. If socket is set to
     *  nonblocking or times out, NSAPI_ERROR_WOULD_BLOCK is returned
     *  immediately.
     *
     *  @param address  The SocketAddress of the remote host.
     *  @param data     Buffer of data to send to the host.
     *  @param size     Size of the buffer in bytes.
     *  @return         Number of sent bytes on success, negative error
     *                  code on failure.
     */
    nsapi_size_or_error_t sendto(const SocketAddress &address,
                                 const void *data, nsapi_size_t size);

    /** Receive a datagram and store the source address in address if it's not NULL.
     *
     *  By default, recvfrom blocks until a datagram is received. If socket is set to
     *  nonblocking or times out with no datagram, NSAPI_ERROR_WOULD_BLOCK
     *  is returned.
     *
     *  @note If the datagram is larger than the buffer, the excess data is silently discarded.
     *
     *  @note If socket is connected, only packets coming from connected peer address
     *  are accepted.
     *
     *  @note recvfrom() is allowed write to address and data buffers even if error occurs.
     *
     *  @param address  Destination for the source address or NULL.
     *  @param data     Destination buffer for datagram received from the host.
     *  @param size     Size of the buffer in bytes.
     *  @return         Number of received bytes on success, negative error
     *                  code on failure.
     */
    nsapi_size_or_error_t recvfrom(SocketAddress *address,
                                   void *data, nsapi_size_t size);

    /** Set the remote address for next send() call and filtering
     *  of incoming packets. To reset the address, zero initialized
     *  SocketAddress must be in the address parameter.
     *
     *  @param address  The SocketAddress of the remote host.
     *  @return         0 on success, negative error code on failure.
     */
    nsapi_error_t connect(const SocketAddress &address);

    /** Send a datagram to connected remote address.
     *
     *  By default, send blocks until all data is sent. If socket is set to
     *  nonblocking or times out, a partial amount can be written.
     *  NSAPI_ERROR_WOULD_BLOCK is returned if no data was written.
     *
     *  @note The socket must be connected to a remote host before send() call.
     *
     *  @param data     Buffer of data to send to the host.
     *  @param size     Size of the buffer in bytes.
     *  @return         Number of sent bytes on success, negative error
     *                  code on failure.
     */
    nsapi_size_or_error_t send(const void *data, nsapi_size_t size);

    /** Receive data from a socket.
     *
     *  This is equivalent to calling recvfrom(NULL, data, size).
     *
     *  If the socket is connected, only packets coming from a connected peer address
     *  are accepted.
     *
     *  By default, recv blocks until some data is received. If socket is set to
     *  nonblocking or times out, NSAPI_ERROR_WOULD_BLOCK can be returned to
     *  indicate no data.
     *
     *  @note recv() is allowed write to data buffer even if error occurs.
     *
     *  @param data     Pointer to buffer for data received from the host.
     *  @param size     Size of the buffer in bytes.
     *  @return         Number of received bytes on success, negative error
     *                  code on failure.
     */
    nsapi_size_or_error_t recv(void *data, nsapi_size_t size);

    /** Not implemented for RAWSocket.
     *
     *  @param error      Not used.
     *  @return           NSAPI_ERROR_UNSUPPORTED
     */
    Socket *accept(nsapi_error_t *error = NULL);

    /** Not implemented for RAWSocket.
     *
     *  @param backlog    Not used.
     *  @return           NSAPI_ERROR_UNSUPPORTED
     */
    nsapi_error_t listen(int backlog = 1);

#if !defined(DOXYGEN_ONLY)

protected:
    nsapi_protocol_t get_proto();

private:
    /** Create a socket out of a given socket
     *
     *  To be used within accept() function. Close() will clean this up.
     */
    RAWSocket(RAWSocket *parent, nsapi_socket_t socket, SocketAddress address);

#endif //!defined(DOXYGEN_ONLY)
};


#endif

/** @}*/
