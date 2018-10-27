> This is not ready yet and highly experimental.

# RADAU

For short introduction see file [DESCRIPTION](DESCRIPTION).

> "Radau" is a German term which means "noise" like caused by a "racket" or a "row".
>
> Radau sends out N+R packets per second, and will receive N+R packets in average.
> This is intentional to keep a stable network data rate.
> [..]
> Today it only offers an archive of network reachability and health.

# State

- Communication port hardcoded
- UDP only
- Linux only
- Encryption missing
- Options missing
- Control missing

# Usage

	git clone https://github.com/hilbix/radau.git
	cd radau
	make
	sudo make install
	su - nobody
	radau other.example.com

# Options

Give known other systems on commandline>

# Files

- `/var/tmp/radau/` keeps all runtime data between boots
- `/var/tmp/radau/sock` is local communication socket
- Files are subject to change

# Control

Control is done over stdin/stdout and the control socket.

To connect to the socket just do `socat - /var/tmp/radau/sock`


# Notes

- `radau` normally uses UDP port 19162 for communication.  This is 0x4adau (where 4 stands for R).
  - Currently this port is unassigned by IANA.  If somebody can assign it to `radau`, many thanks ;)

- `radau` can communicate over any other IP/port.
  - In future TCP and ICMP may be added as a protocol, too.
  - In future packet encapsulation (mimic other protocol) may be added, too.

- `radau` uses encryption for all of it's packets.
  - All packets have the format `NONCE[8] DATA[N]`
  - `NONCE[8]` is a 64 bit NONCE
  - `DATA[N]` is a fixed amount of encrypted data
  - `DATA` is encrypted with some algorithm `ENC` by the `NONCE` and a secret `KEY`
  - There is no indicator which `ENC` is used.
  - There is no indicator which `KEY` is used.
  - The decrypted `DATA` has the format `PROTO[2] PAYLOAD[P] CRC[2]`
  - The default `PROTO` is `0xACD6` to allow to detect a probably successful decryption.  You can change that if you like.
  - `CRC` is a `CRC16` to check for a successful decryption of the completely decrypted `DATA`.
  - `PAYLOAD` has the structure `LENGTH[1] TYPE[1] DATA[LENGTH-2]`
  - `LENGTH==1` means `PADDING`, or a `NOP` payload.  It can be anywhere.
  - `LENGTH==0` means `DATA` is the all the `PAYLOAD` up to the CRC
  - `TYPE` is dependent on the `PROTO`.  With the default `PROTO` see below

- Pure ANSI-C for maximum portability

# License

This Works is placed under the terms of the Copyright Less License,
see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.

Read: This is free as in free beer, free speech and free baby.

