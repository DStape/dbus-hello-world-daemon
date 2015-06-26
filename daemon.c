#include "daemon.h"

/* TODO
 * -make syslog level configurable
 * -use strerror_r() instead of strerror()
 */

void daemonize(void)
{
	/* 0 returned in child */
	if (fork() != 0) {
		exit(EXIT_SUCCESS); /* parent exit */
	}

	/* Become new process group leader */
	if (setsid() < 0) {
		syslog(LOG_NOTICE, "setsid() failed. Error: %\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Overwrite parent's cwd */
	if(chdir("/") < 0) {
		syslog(LOG_NOTICE, "chdir() to root directory failed. Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	/* Close standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	/* Reset file permissions */
	umask(0);

	openlog("helloworld_dbus_daemon", LOG_ODELAY, LOG_NOTICE);
}

