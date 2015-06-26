# Extremely basic Makefile to generate dbus_service_helloworld binary

all:
	gcc -W -Wall dbus_service.c main.c daemon.c `pkg-config --cflags --libs dbus-1` -o dbus_helloworld_service

clean:
	rm -f dbus_helloworld_service