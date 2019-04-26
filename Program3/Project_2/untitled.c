#include <stdio.h>
 #include <stdlib.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netdb.h>
 #include <string.h>
 #include <sys/time.h>
 #include <unistd.h>
 #include "program3Functions.h"


 #define MAX_LINE 4096

 int lookup_and_connect( const char *host, const char *service );

 int main( int argc, char *argv[] ) {

    char* host;
    char* port_number;
    char* file_name;
    char buf[MAX_LINE];
    int s;
    char Errorcheck[MAX_LINE] = "Server error";
    FILE *fptr;
    int bytes_recv = 0;
    int seq[1] = {0};

    if ( argc == 4 ) {
      host = argv[1];
      port_number = argv[2];
      file_name = argv[3];
    } else {
      fprintf( stderr, "usage: %s host\n", argv[0] );
      exit( 1 );
    }


    if ( ( s = lookup_and_connect( host, port_number) ) < 0 ) {
      exit( 1 );
    }
    if ( deliverFilename( s, file_name) == -1 ) {
      perror( "stream-talk-client: send" );
      close( s );
      exit( 1 );
    }

    fptr = fopen(file_name, "w");
    int fd = fileno(fptr);
    memset(buf, 0, sizeof(buf));
    while((bytes_recv = recv(s, buf, sizeof(buf), 0)) > 0){
      if(buf[0] == seq[0]){
        write(fptr, buf + 1, bytes_recv - sizeof(buf[0]));
        seq[0]++;
      }
      packetErrorSend(s, seq, sizeof(seq), 0);
    }
    close(fptr)
    close(s);
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