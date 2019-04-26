EXE = snw_client snw_server 

CFLAGS = -Wall

all : $(EXE)

snw_client: snw_client.c
	gcc -o snw_client snw_client.c -L. -lprogram3Functions -Wall

snw_server: snw_server.c
	gcc -o snw_server snw_server.c -L. -lprogram3Functions -Wall
clean:
	rm -f $(EXE)

