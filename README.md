Radau is an unidirectional network connection tester

> This needs Python3.4 or above

# Planning phase

As I discovered asyncio on Python 3 I decided to give this a try.

Hence I currently rethink everything from scratch.

This is developed without plan, just as it comes and is most easy to implement.


# Usage

```bash
git clone https://github.com/hilbix/radau.git
cd radau
./radau.py some.other.host:port
```

Usually `port` is 19162.  If you leave out the port (or give it as 0) it is understood as 19162 (if not changed in the configuration).

Please note that 19162 stands from the C number `0x4adau`


# About

In German "Radau" means something like "to make a row".  This is exactly what this does.

Each second, a single UDP packet (called broadcast) is sent unidirectional, each time to another host in the crowd.
Received packet transport more information about the crowd, such that everything is autodetecting.

To become part of the crowd, you need a client SSL certificate which is signed by a common CA of all members of the crowd.

Also you need to be able to communicate to the crowd in a bi-directional fashion.  This means, you need to be able to contact others and receive packets from others.  But outgoing and incoming need not be the same other host.

For example if you are behind a firewall and you can send out packets to the Internet but not receive anything, this is OK, as long as there is another local host which is able to receive from the Internet and send data to you.

Usually starting to become part of the cloud is to just know somebody else in the crowd.  However if you are not able to contact any such host, the link can be started by somebody else.


# Configuration

Radau has a runtime directory (by default `~/.config/radau/`) which contains:

- The CA's certificate
- Your (client) certificate, signed by the CA's certificate
- A manual configuration file, which may be missing
- Learned data which may be missing or empty
- Some other information, like a starting value for the NONCE generator

When it starts, it first reads all learned data.  Then it reads the manual configuration (which might override some bits of the learned data).  Then it starts to run.

By default it listens on port 19162, however this can be canged in configuration.


# Broadcast

The broadcast packet looks like following.  The first thingie (starting with `!`) is Python's `struct` format specifier for the field.  BTW, everything is in network byte, hence the `!`.

- Unencrypted Header (8 byte)
  - `!H` Signature, 0x4ADAu
  - `!B` Packet type
  - `!B` Packet subtype
  - `!B`: length of unencrypted data
  - `!3x`: to be defined
- Unencrypted Data (0, 16..255 byte)
  - `!16B`: NONCE, randomly generated for each packet
  - `!16B`: SHA128 
  - payload depending of packet type
- Encrypted Header
  - `!d`: timestamp from `time.time()` when the packet was created
  - `!8B` 8 Monitor categories `000` to `111`.  Transmitted as 8 times 255 if not available.  Values above 254 are capped at 254.
- Encrypted payload
  - payload depending on packet type

# Diagnostic

Radau writes all diagnostic information to `stderr`.


# Future

Following will be implemented in future:


## Monitor

Monitprs are indicators which are taken from time to time, for health check etc.  The outcome of all monitors is categorized into 8 categories.  The summary of all 8 categories are part of the broadcast.

- `OK   0`: 010 Green:  Everything ok
- `PRE  1`: 011 Cyan:   Pre-Warn
- `WARN 2`: 110 Yellow: Warning
- `ERR  3`: 101 Purple: Error
- `KO   4`: 100 Red:    Fatal
- `OFF 15`: 001 Blue:   Offline
- `NO  22`: 000 Black:  Deactivated
- `      `: 111 White:  Unknown

Category colors are easy to understand:

- Is the blue component on, something is in a "probably" state.
- Is the red component on, you need to worry
- Is the green component on, things are working

The periodics can be implemented as external scripts.  In that case the script can do following:

- Output nothing on stdout and stderr, return a return code as number above.

- Output something to stderr which cannot be interpreted.  In that case it is 'KO'.

- Output something to stdout which cannot be interpreted.  In that case it is 'ERR'.

- Output lines of the form `WORD[number][.NAME[.SUB]*][ TEXT]` to stdout, this way it can give multiple responses at the same time
  - `WORD` is one of the words given first for the categories above.
  - `TEXT can contain TAB characters to separate the information into table like columns for display
  - `NAME` is a grouping name of the periodic
  - `SUB` is a sub-grouping name of the periodic (can be present  multiple times)
  - `number` is a number which indicates the timeout of the periodic.  It it does not answer in-time, it is considered `KO`.


## CLI

Argument `-` will start a CLI with Radau reading commands from `stdin` and writing the result to `stdout`.

This CLI can be attached to the network by listening on a socket (Unix or TCP).  More than one socket connection can be given.

Radau also will process any argument which is not `-` and does not contain a `:` as a (single) CLI command.

You can run multiple Radaus in parallel, the first one will run the Radau service in background, all others will re-use this service.

The most important command will be `help`, as this will print out all the possible commands and will be able to explain them.


## Message

A message is something which travels from one crowd member to another one, possibly using other crowd members to reach the destination.

These messages are acknowledged by the destination, such that you know that is was received properly.

Note that messages will travel much faster than just one packet a second.


## Service

On top of the messages Radau can offer named services to others in the crowd.  This way another Radau can run the given command and transfer data as well as receive data from the service.

Service may send out measurement data, like sensor information, act as a tunnel to some other network endpoint or run arbitrary commands on the remote.

Keep in mind, that you do not need a bi-directional nor stable connection for this.  For example, if you open a tunnel to `ssh`, this tunnel stays open regardless of network outages or IP changes at your side or the other side.  If one side drops out of the crowd things just wait until the other side comes back.  Then everything will continue, as if nothing had happened.

Note: If you are using `ssh`, be sure to disable `ServerAliveInterval`, else the `ssh` session will be affected by network outages due to the timeout.


## Hole

A hole is something, which accepts packets for some others of the crowd, and forwards these packets to them.  Holes are just transports, not proxies.  Usually they are unidirectional, so things either fall into a hole or out of a hole.  Note that "hole!" also is the German imperative to go and collect someting.

