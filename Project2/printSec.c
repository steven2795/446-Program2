#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define STDIN 0
#define MAX 4097
#define SERVER_PORT "5432"
#define MAX_LINE 256
#define MAX_PENDING 5



int bind_and_listen( const char *service );

int main(int argc, char *argv[]){

	
	struct timeval screen;
	char buf[MAX];
	char* someChar;
	fd_set readfds;	
	screen.tv_sec = 2;
	screen.tv_usec = 0;
	int count = 0;
	int i = 0;
	int temp = 0;
	int time = 0;
	FD_ZERO(&readfds);
	char* fileName = argv[1];

	while(1){
		FD_SET(STDIN_FILENO, &readfds);
		if(temp = select(STDIN_FILENO+1, &readfds, NULL, NULL, &screen) == -1)
		{
			printf("error\n");
		}
		if(FD_ISSET(STDIN_FILENO, &readfds)){
			printf("Select return value: %d\n", temp);
			printf("Value of Timeval Structure: %d\n", screen.tv_sec);
			fgets(buf, sizeof(buf), stdin);
			printf("User Input Printed: %s\n",buf);
		}
		screen.tv_sec = 2;
		screen.tv_usec = 0;
		
	}
	
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
