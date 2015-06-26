#include "dbus_service.h"

/*
 * TODO
 * write signal handler that cleanly kills off daemon
 */

void handle_dispatch_status(DBusConnection *dconn, DBusDispatchStatus status, void *data)
{
	syslog(LOG_NOTICE, "handle_dispatch_status() called");
}

dbus_bool_t handle_add_watch(DBusWatch *watch, void *data)
{
	syslog(LOG_NOTICE, "handle_add_watch() called");
	return TRUE;
}

void handle_remove_watch(DBusWatch *watch, void *data)
{
	syslog(LOG_NOTICE, "handle_remove_watch() called");
}

void handle_watch_toggled(DBusWatch *watch, void *data)
{
	syslog(LOG_NOTICE, "handle_watch_toggled() called");
}

dbus_bool_t handle_add_timeout(DBusTimeout *timeout, void *data)
{
	syslog(LOG_NOTICE, "handle_add_timeout() called");
	return TRUE;
}

void handle_remove_timeout(DBusTimeout *timeout, void *data)
{
	syslog(LOG_NOTICE, "handle_remove_timeout() called");
}

void handle_timeout_toggled(DBusTimeout *timeout, void *data)
{
	syslog(LOG_NOTICE, "handle_timeout_toggled() called");
}

void handle_free_data(void *mem)
{
	syslog(LOG_NOTICE, "handle_free_data() called");
}

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

	///TODO: check return value of following and act appropriately
	dbus_connection_set_dispatch_status_function(dconn, handle_dispatch_status, NULL, handle_free_data);

	dbus_connection_set_watch_functions(dconn, handle_add_watch, handle_remove_watch, handle_watch_toggled, NULL, handle_free_data);

	dbus_connection_set_timeout_functions(dconn, handle_add_timeout, handle_remove_timeout, handle_timeout_toggled, NULL, handle_free_data);


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

	/* If the connection is closed, the function returns FALSE.
	 *
	 * 0 is max time to block
	 */
	while(dbus_connection_read_write_dispatch(dconn, 0)) {
	}
}
