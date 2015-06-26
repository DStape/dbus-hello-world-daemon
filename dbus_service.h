#include <dbus/dbus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

/* prototypes */
extern DBusConnection * dbus_service_setup(void);
extern void dbus_service_listen(DBusConnection *);
