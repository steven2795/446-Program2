// Tyler Barnes
// Brad Fischer
// CSCI 446
// Srping 2019
/* This code is an updated version of the sample code from "Computer Networks: A Systems
 * Approach," 5th Edition by Larry L. Peterson and Bruce S. Davis. Some code comes from
 * man pages, mostly getaddrinfo(3). */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include "program3Functions.h"
#include <sys/time.h>
#include <sys/select.h>

#define MAX_LINE 1024

/*
 * Lookup a host IP address and connect to it using service. Arguments match the first two
 * arguments to getaddrinfo(3).
 *
 * Returns a connected socket descriptor or -1 on error. Caller is responsible for closing
 * the returned socket.
 */
int lookup_and_connect( const char *host, const char *service );

int main( int argc, char *argv[] ) {
	char *host;
	char *file_name;
	char buf[MAX_LINE];
	int s;
	//int len;
	char *SERVER_PORT;
	int fdo;
	int count;
  char error;
  int ack[1] = {0};
  //int ack_len = 1;
	//int ack_num = 0;

	if ( argc == 4 ) {
		host = argv[1];
		SERVER_PORT = argv[2];
		file_name = argv[3];
	} else {
		fprintf( stderr, "Incorrect amount of command line values given\n");
		exit( 1 );
	}

	/* Lookup IP and connect to server */
	if ( ( s = lookup_and_connect( host, SERVER_PORT ) ) < 0 ) {
		printf("Client Error: Unable to connect to server\n");
		exit( 1 );
	}

	// Main loop: get and send lines of text
  error = deliverFilename(s, file_name);
  if (error == -1) {
    printf("Client Error\n");
    exit(1);
  } else if (error == -2) {
    printf("Servor Error\n");
    exit(1);
	} else {
		//fdo = open("temp.out", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR, 0644);
		fdo = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR, 0644);
		while ((count = recv( s, buf, sizeof( buf ), 0))) {
			if(buf[0] == ack[0]) { 
				write(fdo, buf + 1, count - sizeof(buf[0]));
				ack[0]++;
			}
			packetErrorSend(s, ack, sizeof(ack), 0);
			//send(s, ack, sizeof(ack), 0);
		}
	close(fdo);
	}
	close( s );
	exit(0);
	return 0;
}

int lookup_and_connect( const char *host, const char *service ) {
	struct addrinfo hints;
	struct addrinfo *rp, *result;
	int s;

	/* Translate host name into peer's IP address */
	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	if ( ( s = getaddrinfo( host, service, &hints, &result ) ) != 0 ) {
		fprintf( stderr, "stream-talk-client: getaddrinfo: %s\n", gai_strerror( s ) );
		return -1;
	}

	/* Iterate through the address list and try to connect */
	for ( rp = result; rp != NULL; rp = rp->ai_next ) {
		if ( ( s = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol ) ) == -1 ) {
			continue;
		}

		if ( connect( s, rp->ai_addr, rp->ai_addrlen ) != -1 ) {
			break;
		}

		close( s );
	}
	if ( rp == NULL ) {
		perror( "stream-talk-client: connect" );
		return -1;
	}
	freeaddrinfo( result );

	return s;
}
