#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "program3Functions.h"
#include <fcntl.h>


#define MAX_LINE 4096
#define MAX_PENDING 5



int bind_and_listen( const char *service );

int main( int argc, char *argv[] ) {
    char* port_number;
    char buf[MAX_LINE];
    int s, new_s;
    int len = 0;
    int fileName;
    fd_set rfds;
    struct timeval tv;
    int seq[1] = {0};
    
    if(argc == 2){
         port_number = argv[1];
     }
     /* Bind socket to local interface and passive open */
    if ( ( s = bind_and_listen( port_number ) ) < 0 ) {
                 exit( 1 );
    }
    while ( 1 ){ 
        if ( ( new_s = accept( s, NULL, NULL ) ) < 0 ) {
            perror( "stream-talk-server: accept" );
            close( s );
            exit( 1 );
        }
        FD_ZERO(&rfds);
        FD_SET(new_s, &rfds);
        len = receiveAndVerifyFilename( new_s, buf) ;
        if(len == -1){
            printf("ERROR RECEIVING THE FILE NAME\n");
            exit(1);
        }
        else if(len == -2){
            printf("ERROR OPENING THE FILE\n");
            exit(0);
            return 0;
        }
        else{
            int byte_size = 0;
            fileName = open(buf, O_RDONLY);
            memcpy(buf, seq,sizeof(char) * 4);
            while((byte_size = read(new_s, buf + 1,MAX_LINE - 4))){
                packetErrorSend(new_s, buf, byte_size + sizeof(buf[0]), 0);
                while(select(new_s + 1, &rfds, NULL, NULL, &tv) == 0){
                    packetErrorSend(new_s, buf, byte_size + sizeof(buf[0]), 0);
                    FD_ZERO(&rfds);
                    FD_SET(new_s, &rfds);
                    tv.tv_sec = 0;
                    tv.tv_usec = 50000;
                }
                recv(new_s, buf, sizeof(buf), 0);
                seq[0]++;
            }
        }
    }
        close(fileName);
        close(new_s);
        exit(0);
        return 0;
 }


 // Don't Touch


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
