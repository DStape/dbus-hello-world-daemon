#include "daemon.h"
#include "dbus_service.h"

/*
 * TODO
 * add license to all source files
 * make use of configuration file
 */

int main(void)
{
	daemonize();

	/* at this point we're the child */
	setup_dbus_service();
}
