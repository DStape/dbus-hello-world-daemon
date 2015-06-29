#include <libtac.h>
#include "dbus_service.h"

/*
 * TODO
 * write signal handler that cleanly kills off daemon
 * function prototypes
 * parse args and use these instead of hardcoding server, user, passwd, etc
 * wrap lines at 72
 */

static int connected = FALSE;
static int tac_fd;

void handle_open_conn(void)
{
	struct addrinfo hints;
	struct addrinfo *result;
	int fd;

	if (connected) {
		syslog(LOG_NOTICE, "We're already connected via fd %d", tac_fd);
		return;
	}
	syslog(LOG_NOTICE, "AAAAAAAA");
	memset(&hints, 0, sizeof(struct addrinfo));
	getaddrinfo("localhost", "49", &hints, &result);

	fd = tac_connect_single(result, NULL, NULL);
	if (fd < 0) {
		syslog(LOG_NOTICE, "Failed to connect to server");
		close(fd);
		connected = FALSE;
	}
	syslog(LOG_NOTICE, "We connected!");
	tac_fd = fd;
	connected = TRUE;
}

void handle_do_auth(void)
{
	int reply;
	int fd = tac_fd;

	if (!connected) {
		syslog(LOG_NOTICE, "Not yet connected to server");
		return;
	}
	/* tac_auth_send(...[3]...) (tty):
	 *
	 * The ASCII name of the client port on which the authentication is
	 * taking place. The value of this field is client specific. (For
	 * example, Cisco uses "tty10" to denote the tenth tty line and
	 * "Async10" to denote the tenth async interface).
	 *
	 * tac_auth_send(...[4]...) (r_addr):
	 *
	 * An ASCII string that describes the user’s remote location.
	 * This field is optional (since the information may not be available).
	 *
	 */
	if (tac_authen_send(tac_fd, "testuser", "testpassword", "", "") < 0) {
		syslog(LOG_NOTICE, "Failed to send auth request to server");
		close(fd);
		connected = FALSE;
	}

	reply = tac_authen_read(fd);
	if (reply == TAC_PLUS_AUTHEN_STATUS_PASS) {
		syslog(LOG_NOTICE, "Authentication success!");
	}
	else {
		syslog(LOG_NOTICE, "Authentication failed.");
	}
}

void disconnect_from_server(void)
{
	if (connected) {
		/* TODO check return value */
		close(tac_fd);
		connected = FALSE;
		syslog(LOG_NOTICE, "Disconnection successful");
	}
	else {
		syslog(LOG_NOTICE, "We're not connected.");
	}
}

void stop_service(DBusConnection *dconn)
{
	disconnect_from_server();
	dbus_connection_unref(dconn);
}

DBusHandlerResult handle_message(DBusConnection *connection, DBusMessage *message, void *user_data)
{
	/* TODO: case statement on method? */
	if (dbus_message_is_method_call (message, "com.example.HelloWorld", "Connect")) {
		syslog(LOG_NOTICE, "Connect method called.");
		handle_open_conn();
	}
	else if (dbus_message_is_method_call (message, "com.example.HelloWorld", "Authenticate")) {
		syslog(LOG_NOTICE, "Authenticate method called");
		handle_do_auth();
	}
	else if (dbus_message_is_method_call (message, "com.example.HelloWorld", "Disconnect")) {
		syslog(LOG_NOTICE, "Disconnecting...");
		disconnect_from_server();
	}
	else if (dbus_message_is_method_call (message, "com.example.HelloWorld", "Stop")) {
		syslog(LOG_NOTICE, "Stopping...");
		stop_service(connection);
	}
	else {
		syslog(LOG_NOTICE, "Unkown message received");
	}
	/* TODO free msg and user_data */
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
		syslog(LOG_NOTICE, "We failed to register object path");
	}
	/* TODO: add filters */
	return dconn;
}

void dbus_service_listen(DBusConnection *dconn)
{
	DBusMessage *dmsg;

	/*
	 * If the connection is closed, the function returns FALSE.
	 */
	while(dbus_connection_read_write_dispatch(dconn, 0)) {;}

	syslog(LOG_NOTICE, "Connection closed.");

	/* TODO cleanup */
}
