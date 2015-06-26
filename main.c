#include "daemon.h"
#include <dbus/dbus.h>
#include "dbus_service.h"

/*
 * TODO
 * add license to all source files
 * make use of configuration file
 */

int main(void)
{
	DBusConnection *dconn;
	daemonize();

	/* at this point we're the child */
	dconn = dbus_service_setup();
	dbus_service_listen(dconn);
	exit(EXIT_FAILURE);
}
