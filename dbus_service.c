#include "dbus_service.h"

/*
 * TODO
 * write signal handler that cleanly kills off daemon
 */

DBusHandlerResult handle_message(DBusConnection *connection, DBusMessage *message, void *user_data)
{
	syslog(LOG_NOTICE, "handle_message(...) invoked!");
	return DBUS_HANDLER_RESULT_HANDLED;
}

DBusConnection * dbus_service_setup(void)
{
	DBusConnection *dconn;
	DBusError derr;
	DBusObjectPathVTable vtable;
	dbus_bool_t dret;
	int ret;

	dbus_error_init(&derr);

	/* We want all users to use this service, hence connect to system bus.*/
	dconn = dbus_bus_get(DBUS_BUS_SYSTEM, &derr);

	if (dbus_error_is_set(&derr)) {
		syslog(LOG_NOTICE, "Failed to connect to system bus: %s", derr.message);
		dbus_error_free(&derr);
		exit(0);
	}

	/* Request a well-known bus name.
	 *
	 * We only care about being the primary owner.
	 */
	ret = dbus_bus_request_name(dconn, "com.example.HelloWorld", DBUS_NAME_FLAG_DO_NOT_QUEUE, &derr);

	if (dbus_error_is_set(&derr)) {
		syslog(LOG_NOTICE, "Failed to acquire com.example.HelloWord. %s", derr.message);
		dbus_error_free(&derr);
	}

	if(ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
		syslog(LOG_NOTICE, "We failed to become the primary owner :(");
		exit(EXIT_FAILURE);
	}

	/* Setup function handlers */
	memset(&vtable, 0, sizeof(vtable));
	vtable.message_function = handle_message;

	dret = dbus_connection_register_object_path(dconn, "/com/example/HelloWorld", &vtable, NULL); //NULL is user_data
	syslog(LOG_NOTICE, "dret false? %d\n", dret==FALSE);
	if (!dret) {
		syslog(LOG_NOTICE, "We failed to register object path: %s", \
							dret == DBUS_ERROR_OBJECT_PATH_IN_USE ? \
							"DBUS_ERROR_OBJECT_PATH_IN_USE" : \
							"DBUS_ERROR_NO_MEMORY");
	}
	return dconn;
}

void dbus_service_listen(DBusConnection *dconn)
{

	DBusMessage *dmsg;

	/* If the connection is closed, the function returns FALSE.
	 *
	 * 0 is max time to block
	 */
	while(dbus_connection_read_write_dispatch(dconn, 0)) {

		/*
		 * dbus_connection_pop_message() is only intended for very simple
		 * applications, which this is. This function is incompatible
		 * with registered message handlers as it will simply bypass them.
		 *
		 * TODO: use function handlers instead
		 */
		/*dmsg = dbus_connection_pop_message(dconn);
		if (dmsg == NULL) {
			/* queue is empty *
			continue;
		}
		else {
			syslog(LOG_NOTICE, "msg type: %d", dbus_message_get_type(dmsg));
			syslog(LOG_NOTICE, "object path: %s", dbus_message_get_path(dmsg));
			syslog(LOG_NOTICE, "interface: %s", dbus_message_get_interface(dmsg));
			syslog(LOG_NOTICE, "destination: %s", dbus_message_get_destination(dmsg));
			syslog(LOG_NOTICE,"sender: %s", dbus_message_get_sender(dmsg));
		}

		/*
		 * Expose two methods on the com.example.HelloWorld interface
		 * 
		 * [1]=interface, [2]=method
		 *
		if (dbus_message_is_method_call(dmsg, "com.example.HelloWorld", "Echo")) {
			syslog(LOG_NOTICE, "Hello, world!");
		}
		else if (dbus_message_is_method_call(dmsg, "com.example.HelloWorld", "Exit")) {
			syslog(LOG_NOTICE, "Goodbye.");
			dbus_message_unref(dmsg);
			closelog();
			exit(EXIT_SUCCESS);
		}
		/* When count reaches 0, free the message*
		dbus_message_unref(dmsg);
		*/
	}
}
