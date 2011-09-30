Internals
=========

Radau sends out one UDP packet to all configured destinations.  Also it receives UDP packets from anywhere.

On average one packet is sent and one packet is received.

A packet contains:

- 1: Packet version (hex 10)

- list: encryption/signature information (currently empty)

- 4: timestamp (MSB first) seconds since epoch (1970)

- str: source (usually FQDN, but can be a binary symbolic number like UUID)

- list: list of source IP addresses with prefix length

- list: list of timestamps heared from destination

- list: Information about other heared destinations

- list: Additional payloads

- 2+2: Checksum (Adler32) (B and A, MSB first)

The packet contents as well as the length varys, such that MTU problems can be automatically be discovered and circumvented.

str has the format "num data" where num is the length of the string

list is in the format "cnt str..." where cnt gives the length of the list in bytes.  The list entries each are strings, but the strings can be data.

"num" is transmitted byte by byte, MSB first in 7 bit portions.  The MSB of each byte states that another byte follows (1) or this is the last byte (0).

So 0 is transmitted as 0, 127 as 127, 128 as 129 0, 255 as 129 127, 65535 as 131 255 127 and so on.  The maximum possible number has 64 bits.

Note on encryption:  All data except Packet version, Encryption Information and Checksum will be encrypted.  Encryption information will contain an IV.
