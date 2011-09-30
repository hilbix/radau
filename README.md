Radau is an unidirectional connection test network.

Pre-Alpha
=========

Radau is in an early development state.  Most of the final features are not yet implemented nor documented.

Until the final license is found, this can be copied according to GPL v2 with Copyright (C) 2011 by Valentin Hilbig.

Note that this Copyright is missing in the sources.  This is not a bug, because in Germany you do not need to add a Copyright to have it.  If you move this to another jurisdiction, you should add the missing Copyright (and GPL license file) in the sources.

(Perhaps I decide to make it CLL.  We will see.)


About
=====

If you run a handfull of servers for some years like me, you will know the situation:

A machine becomes unreachable and you don't know why.  Is it down?  Or is the ISP down?  Is just some Link down?  Or is it still reachable, but with extreme packet loss rate?


With Radau you can easily spot, for example, that a machine still has a route into Internet but the back route is failing.  Maybe the link is down or saturated into one direction, like in a DDoS situation.  Perhaps the machine can still talk to other machines on the local LAN but nowhere else, so there might be an open open route to the machine.

With Radau you can quickly identify these cases and routes.

Note that "Radau" is German for "noise", the name comes from the fact that this programs makes permanent noise on the network.  An alternative name for this was "Radar" or "Sonar", but it does not use a standard reflection like "ping", so this name would be misleading.


Intall
======

This uses cogen with greenlets.  Greenlets are coroutines as they ought to be.

```bash
apt-get install python2.6-dev python-greenlet
easy_install cogen

git clone https://github.com/hilbix/radau
cd radau
vi radau.cfg
```

radau.cfg is made of config lines with space arguments:

# comment
listen IP:PORT
listen6 IP:PORT
host HOSTNAME:PORT PASSWORD

Lines starting with `#` and empty lines are ignored.

IP:PORT is optional.  PORT defaults to 0x4adau in C notation, which is 19162.  IP defaults to '' which is the same as '*' which means 'all local IPs'.  It can be a resolveable hostname, too.

HOSTNAME can be either an IP, a resolveable hostname or "*" for hosts with dynamic IP.  PORT is optional and defaults to 0x4adau in C notation (19162).
PASSWORD is an optional shared secret.

You must run Radau on each destination for it to be able to handle everything.


Usage
=====

Start it:
	./radau.py

Status
======

- This is not yet usable

- Password / Encryption is not yet implemented.  Authentication must be implemented to lock out others disturbing Radau.

- The current time resoltion is 1s

