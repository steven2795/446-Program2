#include <sys/types.h>
#include <sys/socket.h>

#define MAX_PACKET_DATA_SIZE (1400)
#define MAX_FILENAME_LENGTH (MAX_PACKET_DATA_SIZE-1)

/*
 * deliverFilename will correctly send the filename to the server and receive
 * any indication of error from the server. deliverFilename will never experience a
 * packet loss. The return value indicates the type of error that occurs, if any.
 * Only the client should use deliverFilename.
 *
 * INPUTS: sockfd - the socket descriptor for the valid, connected socket to use
 *           for communication with the server
 *         filename - the filename as a NULL terminated string to send to the server
 *           of at most MAX_FILENAME_LENGTH characters (excluding NULL)
 *
 * RETURN:  0 upon success (filename sent to server and no error at server)
 *         -1 if a client error occurs (e.g., bad socket)
 *         -2 if a server error occurs (e.g., file does not exist, can't open file)
 */
char deliverFilename(int sockfd, const char *name);

/*
 * receiveAndVerifyFilename will accept a filename over the socket sockfd, verify the
 * server can open the file, and send an error indication over the socket if an error
 * occurs. The filename received from the client is returned in filename, which must
 * contain enough space to hold MAX_FILENAME_LENGTH+1 characters.
 * receiveAndVerifyFilename is the partner to deliverFilename. The return value
 * indicates if an error occurs. Only the server should use receiveAndVerifyFilename.
 * Upon a successful return, the server should be able to open the file, but the
 * server program must still open and read the file.
 *
 * INPUTS: sockfd - the socket descriptor for the valid, connected socket to use
 *           for communication with the client
 *         filename - location to store the received filename, which must be at least
 *           MAX_FILENAME_LENGTH+1 characters
 *
 * RETURN: 0 upon success (filename received and file can be opened)
 *        -1 if there was an error receiving the filename (e.g., bad socket, recv error)
 *        -2 if there was an error opening the file (e.g., doesn't exist, permissions)
 *        -3 if there was an error sending the response to the client
 */
char receiveAndVerifyFilename(int sockfd, char *filename);

/*
 * A function that randomly drops a packet. packetErrorSend has the same
 * format and arguments as send(2). Packets are capped at
 * MAX_PACKET_DATA_SIZE bytes.
 *
 * When packetErrorSend drops a packet, it will return len, up to a max
 * of MAX_PACKET_DATA_SIZE, as if the packet was actually sent.
 *
 */
ssize_t packetErrorSend(int sockfd, const void *buf, size_t len, int flags);
