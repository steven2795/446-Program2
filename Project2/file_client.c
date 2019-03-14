#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE 256

int lookup_and_connect( const char *host, const char *service );

int main( int argc, char *argv[] ) {

	char* host;
	char* port_number;
	char* file_name;
	char buf[MAX_LINE];
    int s;
    int len;
    int i = 1;
	
	if ( argc == 4 ) {
        host = argv[1];
		port_number = argv[2];
		file_name = argv[3];

    }
    else {
        fprintf( stderr, "usage: %s host\n", argv[0] );
        exit( 1 );
    }


	 /* Lookup IP and connect to server */
    if ( ( s = lookup_and_connect( host, port_number) ) < 0 ) {
        exit( 1 );
    }
	
    while ( i > 0 ) {
        if ( send( s, file_name, strlen(file_name), 0 ) == -1 ) {
            perror( "stream-talk-client: send" );
            close( s );
            exit( 1 );
        }
        i--;
    }

	
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
   
