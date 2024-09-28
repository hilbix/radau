# Radau spec

Note that `radau` trusts all scripts.

Hence each script has full power to issue commands.

If you do not want that, implement firewall scripts.
You can do this with a dedicated script-to-script communication.

## Threads

Five basic threads

### Main Thread

Manages and supervises all the threads.

### Log Thread

Output to STDERR or file

### Send Thread

- Sends out a single packet each second
- In fast mode it sends more

### Receive Thread

- Processes incoming packets
- Hands them to the Receive Script

### IO thread

Synchronously process IO

- One IO thread per forked script
- One IO thread for interactive use (STDIN/STDOUT)

### Cryptographic thread

- All cryptographic functions are bundled here
- This is needed, as OpenSSL is not thread-aware


## Scripts

Note that scripts are usually `bash` scripts
which receive information on STDIN
and output commands to the kernel using STDOUT.

But it can be real programs, too.

Scripts get 4 file descriptors:

- 0 STDIN: PIPE from the kernel for Reply/Requests
- 1 STDOUT: PIPE to the kernel for Requests/Reply
- 2 STDERR: same as STDERR of `radau`
- 3 BINARY: unix domain socket
  - for binary data and file descriptor passing
  - or some other socket connection

### Configuration

> `script/radau-config.sh`

- Forked by main thread
- Only a single script runs in parallel
- Reads the configuration
- Hands the configuration to the main thread
- Then waits for configuration changes
- Updates the permanent configuration

You should use the standard script, because this probably breaks future improvements.

### Mesh Routing Script

> `script/radau-mesh.sh`

- Forked by main thread
- Only a single script runs in parallel
- Sends out mesh queries to other nodes
- Receives mesh updates from other nodes
- Receives mesh queries from the kernel
- Answers with the destination (IP) to try
- Can be killed, so it is restarted any time
- The kernel caches destination information until this script updates them
- Can implement a permanent helper database
- Should update important configuration via Configuration script
- But usually keeps all information volatile in RAM

You can customize this script to your meshing needs.


### Receive Script

> `script/radau.sh`

- Forked by main thread
- More than one Receive Script can be running in parallel
- Run on-demand if needed
- Used by Receive Thread
- Used for binary transfer and more
- It is up to you what this script does


## Script Protocol

The protocol is plain ASCII lines ending on CR or LF.
If a stray LF is received after an CR this is ignored.
A stray LF on the line without a preceeding CR (empty line) is an error.

- Arguments are whitespace separated.
  - This includes TABs, however the kernel always uses SPC only
  - Empty arguments are not supported
  - Hence multiple whitespace cannot happen
  - If the kernel receives multiple whitespace this is fatal error
- The format always is `COMMAND ARGS..`
- The reply always is `NUMBER ARGS..`
- The protocol is strictly serial
  - There is no inlining or anything else
- The protocol is strictly synchronous
  - It is a strict request/reply pattern
  - Each request gets a single reply
  - Replies can have multiple lines
- Note that lines never start with SPC nor end on SPC
  - You do not need to check that

### COMMANDs

- `0` (or anything starting with 0) is ignored (see `NUMBERs 0`)
  - must be send out by the script in regular intervals
  - to tell the kernel that the script is alive
  - Typically answer for a `0` from the kernel

T.B.D.


### NUMBERs

There are 3 numbers `XYZ` or 4 numbers `XYZZ` followed by a whitespace.

- The blank can be left away if the line ends there

`X` defines the error code:

- `0` ignore this line
  - The kernel ignores such lines received but records the activity as alive-check.
  - The kernel sends out these lines in regular intervals as keepalinve.
  - So if a script does not receive anything within 5 minutes, this is a timeout.
  - Note that the line needs not to be correctly formed, just starting with a `0` is enough
- `1` text reply, more lines follow
  - `1` can be followed by more `1`
  - It must be ended with `2`
  - If it is followed by something else, this is a communication error
- `2` postive or last answer
- `3` negative answer
- `4` failure, connection can continue
  - This needs a resynchronization
- `5` failure, connection must be terminated

`Y` defines the type of data:

- `0` plain arguments
- `1` to `9`: undefined, reserved

`Z` or `ZZ` defines the number of arguments:

- `0` no arguments, ignore the rest of the line
- `1` one argument, exactly
- `2` two arguments, exactly
- and so on up to `99` arguments


### Resynchronization

When a `4XX` is received resynchronization is done as follows:

- An empty line is sent (so just an LF)
- All data which is still buffered is discarded
- All lines received are discarded until an empty line is received
- A `RESYNC` command is sent which includes a NONCE
- It must be replied by `201` with the same NONCE
- We are synchronized again

When a `4XX` is send out:

- All data which is still buffered is discarded
- All lines are discarded until a `RESYNC NONCE` is received
- This is replied with `201 NONCE` with the same NONCE
- We are synchronized again


## Script to Script communication

Script to script communication is supported with the same protocol.
There are two types of communications:

- Inline, which happens over the kernel communication
- Dedicated, which happens on additional file descriptors
- Exclusive, which happens instead of the kernel descriptors

Dedicated and Exclusive only work if a new script is forked or file descriptor passing is implemented (as of today it isn't).

The kernel checks the protocol, only the request/reply flow can switch sides.

Binary data transfer is supported.  But only a single binary transfer can happen at any time,
so parallel binary data transfer from multiple script-to-script connection is not available.
You need to fork additional scripts for this (or implement your own type of multiplexing).


## Binary data transfer

The binary data transfer is synchronous and controlled.

- The kernel only supports one way binary data transfer.
- However for script-to-script data can flow into both directions simultanously.
- The validity of the data transfer is checked by the kernel, too.

The binary data transfer always has a predefined length which is negotiated with the commands.
If you do not know the size in advance, you have to initiate multiple transfers.

The protocol flow is as follows:

- Request is received, it includes the size of the data
- Reply is sent out
- If the reply is not positive (`2`), the data transfer is aborted

Note that the Request/Reply is independent of the data transfer,
so additional Request/Reply can happen while the data transfer is active.
For example, the receiver can send out progress information etc.

However the kernel ensures, that another data transfer in the same direction will block the communication
until the previous data transfer is finished.  So the script will not receive the request before data transfer is complete.

The binary data is sent out as follows:

- The binary data of the given length
- A hex string of the checksum plus a NUL

If the checksum does not match (the kernel ensures that) or something else fails:

- The binary connection file descriptor is closed before the checksum
- The communication is terminated by `500` which includes a possible cause
- The script then can clean up and terminate

