# TACACS+ D-Bus Daemon

# Aim
Initial aim was to implement a simple dbus application (written as a daemon) that exposed one interface allowing other dbus applications to invoke some methods. This has been achieved, please switches to branches dbus\_connection\_pop\_message and register-function-handler for more information.

Next aim is to expose new methods via one interface that makes the daemon connect with a TACAS+ server and perform AAA. So far, username/password authentication can be done. 

# Package requirements
libdbus-1-3, libdbus-1-dev and libtac. The former two should be available in your distribution's official repos. libtac can be cloned from https://github.com/jeroennijhof/pam_tacplus. Follow the steps there for building and installing, although the typical "autoreconf -i ; ./configure ; make ; make install" should suffice.

# TACACS+ server
apt-get install tacacs+
The configuration file exists is /etc/tacacs+/tac_plus.conf. I won't go into detail on all the configuration options, not least because I've only recently starting using this myself. There are many docs and tutorials online, however I found the following site particularly useful: https://networklessons.com/linux/how-to-install-tacacs-on-linux-centos/.

For the sake of simplicity, leave the default configuration to use /etc/passwd as the databse.

# Starting the TACACS+ D-Bus daemon
Simply run ./dbus_helloworld_service. Use the busctl utility to verify that the daemon is up and running and can be reached via the well-known name of "com.example.HelloWorld".

# API
At the moment, there is one interface: "com.example.HelloWorld". This exposes four methods: Connect, Authenticate, Disconnect and Stop.

# com.example.HelloWorld.Connect
Instructs the daemon to connect to the TACACS+ server. At the moment, the location is hard-coded to localhost:49 (TACACS default port, as per RFC-1492).

# com.example.HelloWorld.Authenticate
If a connection has been established, the daemon will send an authentication request with the currently hard-coded values of user="testuser", password="testpassword". (To add a new user, simply run 'sudo adduser testuser' and follow the prompt).

#com.example.HelloWorld.Disconnect
This will terminate the connection with the TACACS+ server, if it has been established.

#com.example.HelloWorld.Stop
The daemon will terminate its connection with the TACACS+ server and will exit.

# Client
You can write your own client or use the dbus-send tool. W.r.t. the latter, to invoke the Echo method, run: "dbus-send --system --dest=com.example.HelloWorld --type=method_call /com/example/HelloWorld com.example.HelloWorld.Connect".

# How do I know it's working?
tail -f /var/log/messages | grep helloworld_dbus_daemon

# Gotchas
TODO, add example conf
You will more than likely need to edit the dbus-service configuration file. This can be found @ /et c/dbus-1/system.d/dbus_service.conf. More info can be found at http://dbus.freedesktop.org/doc/dbus-daemon.1.html.

By default, the TACACS+ server will respond to requests with an encrypted payload. For debugging purposes, this obviously isn't very helpful, unless you give wireshark the key and tell it to do the decryption. To tell the server to not encrypt its packets, simply comment out the line with "key =  xxx".

