Future ideas for Radau

Usage
=====

Start with control interface:
	./radau.py .sock

Connect to the control interface:
	socat - unix:.sock

Assign a port to the control interface:
	socat tcp-listen:1111,bind=127.0.0.1,fork unix:.sock

Look at the control interface using HTTP browser:
	lynx http://127.0.0.1:1111/

Create a point to point connection between two nodes:
-----------------------------------------------------

On NODE1:
	socat - PROXY:127.0.0.1:NODE2:CHANNEL,proxyport=1111

On NODE2:
	socat - PROXY:127.0.0.1:NODE1:CHANNEL,proxyport=1111

The port number is used as virtual channel number.  All data sent from/to a destination is (slowly) forwarded to the other destination, possibly using indirect routes.  Note that all channel listeners participiate on such a channel at each node.

Notes
-----

The control interface acts as a primitive HTTP server and HTTP type proxy.  This way you can point HTTP browsers to it directly or indirectly.  However if the control interface is a unix domain socket, you need to assign a port to the control interface.

For security reasons I do not recommend to use TCP sockets, because these are lacking a proper access control and possibly could be abused from remote using IP and MAC spoofing attacks.


Status
======

- Password / Encryption is not yet implemented.  Authentication must be implemented to lock out others disturbing Radau.

- Implement a primitive PKI such that autolearning of destinations becomes possible.

- STP / routing is not yet implemented.  The idea is that this network finds open routes from any source to any destination and vice versa.  Not neccessarily on the same path, in case that some links are down unidirectional.

- It should run fine behind NAT.  However neither UPnP nor special NAT algorithms will be there, such that two nodes behind NAT each will probably not be able to reach each other.  However they will learn about each other via the other network.

- In future run one thread for each local interface, such that multi homed hosts automagically learn alternative routes.

