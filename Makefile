# Extremely basic Makefile to generate dbus_service_helloworld binary

CC=gcc
CFLAGS=-W -Wall -pedantic

all:
	$(CC) $(CFLAGS) dbus_service.c main.c daemon.c `pkg-config --cflags --libs dbus-1 libtac` -o dbus_helloworld_service

clean:
	rm -f dbus_helloworld_service
