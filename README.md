# Hello world daemon using libdbus

# Aim
Implement a simple dbus daemon that exposes an interface that will allow other dbus applications to remotely invoke the daemon's public methods.

# Package requirements
libdbus-1-3 and libdbus-1-dev (for dev headers)

# Starting the daemon
Simply run ./dbus_helloworld_service. Use the busctl utility to verify that the daemon is up and running and can be reached via the well-known name of "com.example.HelloWorld".

# API
At the moment, there is one interface: "com.example.HelloWorld". This exposes two methods: Echo and Exit.

# com.example.HelloWorld.Echo
When invoked, this will simply print "Hello, world!" to the system log (/var/log/messages).

# com.example.HelloWorld.Exit
Invoking this method will gracefully terminate the daemon

# Client
You can write your own client or use the dbus-send tool. W.r.t. the latter, to invoke the Echo method, run: "dbus-send --system --dest=com.example.HelloWorld --type=method_call /com/example/HelloWorld com.example.HelloWorld.Echo".

# How do I know it's working?
tail -f /var/log/messages | grep helloworld_dbus_daemon

# Gotchas
You will more than likely need to edit the dbus-service configuration file. This can be found @ /et c/dbus-1/system.d/dbus_service.conf. More info can be found at http://dbus.freedesktop.org/doc/dbus-daemon.1.html.
