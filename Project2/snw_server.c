#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "program3Functions.h"


#define MAX_LINE 4096
#define MAX_PENDING 5



int bind_and_listen( const char *service );

int main( int argc, char *argv[] ) {
	char* port_number;
	char buf[MAX_LINE];
	char temp_buf[MAX_LINE];
	int s, new_s;
	int len = 0;
   	char *fileName;
	FILE *fptr;
    char* error = "Server error";

        memset(buf, 0, sizeof(buf));
	if(argc == 2){
		port_number = argv[1];
	}

        /* Bind socket to local interface and passive open */
        if ( ( s = bind_and_listen( port_number ) ) < 0 ) {
                exit( 1 );
        }
        while ( 1 ) {
            if ( ( new_s = accept( s, NULL, NULL ) ) < 0 ) {
                perror( "stream-talk-server: accept" );
                close( s );
                exit( 1 );
            }
            if ( len < 0 ) {
                    perror( "streak-talk-server: recv" );
                    close( s );
                    exit( 1 );
            }
            len = receiveAndVerifyFilename( new_s, buf) ; 
            fileName = buf;
            printf("%s\n", buf);
		    //close( new_s );
		    break;
        }
	fptr = fopen(fileName, "r");
	memset(buf, 0 , sizeof(buf));
	int byte_size = 0;
	int fd = fileno(fptr);
	int seq =  42;
	while((byte_size = read(fd, temp_buf, sizeof(temp_buf))) > 0){
		memcpy(&buf[0], &seq, sizeof(char)*4);
		memcpy(&buf[4], &temp_buf, sizeof(temp_buf));
		printf("buf:%s\n", buf); 
		packetErrorSend(new_s, buf, byte_size + 4, 0);
		memset(buf, 0 , sizeof(buf));
		// select 50ms
		// 	if(ack recv)
		// 	resend (no ack)
		// 	or go on and iterate seq #
	}	

	close(new_s);
	close(s);
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
