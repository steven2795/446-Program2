// Tyler Barnes
// Brad Fischer
// CSCI 446
// Spring 2019
/* This code is an updated version of the sample code from "Computer Networks: A Systems
 * Approach," 5th Edition by Larry L. Peterson and Bruce S. Davis. Some code comes from
 * man pages, mostly getaddrinfo(3). */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "program3Functions.h"
#include <sys/time.h>
#include <sys/select.h>

#define MAX_LINE 1024
#define MAX_PENDING 5

/*
 * Create, bind and passive open a socket on a local interface for the provided service.
 * Argument matches the second argument to getaddrinfo(3).
 *
 * Returns a passively opened socket or -1 on error. Caller is responsible for calling
 * accept and closing the socket.
 */
int bind_and_listen( const char *service );

int main( int argc, char *argv[] ) {
	char buf[MAX_LINE];
	int s, new_s;
	char *SERVER_PORT;
	char file_name[MAX_FILENAME_LENGTH+1];
	int fdi;
	int count;
	int SeqNum[1] = {0};
	char error;
  fd_set rfds;
  struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 50000;

	if (argc == 2) {
		SERVER_PORT = argv[1];
	} else {
		fprintf( stderr, "Incorrect amount of command line values given\n");
		exit( 1 );
	}

	/* Bind socket to local interface and passive open */
	if ( ( s = bind_and_listen( SERVER_PORT ) ) < 0 ) {
		exit( 1 );
	}

	/* Wait for connection, then receive and print text */
	while ( 1 ) {
		if ( ( new_s = accept( s, NULL, NULL ) ) < 0 ) {
			perror( "stream-talk-server: accept" );
			close( s );
			exit( 1 );
		}
		FD_ZERO(&rfds);
		FD_SET(new_s, &rfds);
    error = receiveAndVerifyFilename(new_s, file_name);
    if (error == -1) {
      printf("Error Receiving the File Name\n");
      exit(1);
    } else if (error == -2) {
      printf("Error Opening the File\n");
      exit(0);
      return 0;
    } else if (error == -3) {
      printf("Error Sending Response to Client\n");
      exit(1);
    } else {
    	fdi = open(file_name, O_RDONLY);
			memcpy(buf, SeqNum, sizeof(SeqNum));
			while ( (count = read( fdi, (buf + 1), sizeof(buf) - sizeof(buf[0]))) ) { //read opened file
      	packetErrorSend(new_s, buf, count + sizeof(buf[0]), 0); // send data to client hopefully
				//send(new_s, buf, count + sizeof(buf[0]), 0);
				while (select(new_s + 1, &rfds, NULL, NULL, &tv) == 0) {
					packetErrorSend(new_s, buf, count + sizeof(buf[0]), 0);
					//send(new_s, buf, count, 0);
					FD_ZERO(&rfds);
			 		FD_SET(new_s, &rfds);
					tv.tv_sec = 0;
					tv.tv_usec = 50000;
				}
				recv(new_s, buf, sizeof(buf), 0);
				SeqNum[0]++;
			}
		}
		close(fdi);
		close( new_s );
		exit(0);
	}

	close( s );
	exit(0);
	return 0;
}

int bind_and_listen( const char *service ) {
	struct addrinfo hints;
	struct addrinfo *rp, *result;
	int s;

	/* Build address data structure */
	memset( &hints, 0, sizeof( struct addrinfo ) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;

	/* Get local address info */
	if ( ( s = getaddrinfo( NULL, service, &hints, &result ) ) != 0 ) {
		fprintf( stderr, "stream-talk-server: getaddrinfo: %s\n", gai_strerror( s ) );
		return -1;
	}

	/* Iterate through the address list and try to perform passive open */
	for ( rp = result; rp != NULL; rp = rp->ai_next ) {
		if ( ( s = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol ) ) == -1 ) {
			continue;
		}

		if ( !bind( s, rp->ai_addr, rp->ai_addrlen ) ) {
			break;
		}

		close( s );
	}
	if ( rp == NULL ) {
		perror( "stream-talk-server: bind" );
		return -1;
	}
	if ( listen( s, MAX_PENDING ) == -1 ) {
		perror( "stream-talk-server: listen" );
		close( s );
		return -1;
	}
	freeaddrinfo( result );

	return s;
}
