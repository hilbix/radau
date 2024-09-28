> This still is in the planning phase.
> As of today, there is nothing usable here yet.


# Radau

Unidirectional connection mesh using UDP.

In German, "radau" stands for row or noise.   This is exactly what this here is about.
To unidirectionally create (useful) noisy packets on connection lines with a packet rate of just 1 packet per second, in and out.

Sometimes some connections become unidirectional.
Like a tranceiver goes dead etc.
In that case you still want to know that the machine or network is alive.

Or you have a firewall, which might be wrongly constructed,
such that packets can get through in one direction but not the other.
Something you want to check, too.

Also the machines shall be able to talk to each other,
without knowing anything else than their cryptographic key.

And everything should work steadily in the background with a minimum of network load.
So this is not about speed, just about staying informed.
Hence this here only sends out a single packet per second.
And therefor, on average, it will just receive a single packet per second, too.

The kernel itself shall be as small as possible.  And everything else will be handled by external scripts.
Being unidirection means, the connection protocol is UDP.  And in future, it might even disguise as QUIC.


## Usage

	git clone https://github.com/hilbix/radau.git
	cd radau
	make
	./radau


## Parameters

T.B.D.


## Fast mode

Usually `radau` only sends out a single packet each second.

However to support faster binary data transfer between two `radau` instances,
there is a `fast mode`, where packets are sent out faster than that.

In that case two `radau` instances communicate much faster than usally.
Note that the transfer can involve multiple `radau` instances,
as these are meshed.

It is likely that fast mode will be evolve in future:

- Currently it is slow because it has a very basic flow control
  - A packet is sent out
  - And it must be acknowledged by the other side
- In future pipelining might be implemented
  - So multiple packets are sent out until a transfer window has filled
  - very like TCP
- Another optimization might be multi path binary transfer
  - This way you might saturate multiple internet connections in parallel
- There is no flow rate control
  - There should be a way to limit how fast packets can be sent out
  - There also should be a way to proritize packets, such that more important things are sent out first
  - The latter allows to include dummy packets, which make side band analysis more harder


## Crypto

`radau` packets are AES encrypted.  OpenSSL X25519 is used for key exchange.
And all packets are authenticated.  Also the keys can be authenticated.

The encryption always is E2E.  The E2E encryption is handled by the kernel and thus transparent to scripts.

If a packet needs to be forwarded, it is encapsulated into a forward 


However the kernel should not be able to get hold on the decrypted data.  Hence there must be
added some standard commands which are able to handle the data transfer with E2E encryption
and can be invoked on-demand by the scripts.


## Safety

Currently all scripts are trusted.  Which means, all other `radau` in the mesh are trusted, too.

As long as there are not many scripts, this is not a problem, as all the security can be implemented in these scripts.

However on the long run this is bad.  Security must be supported by the kernel.

Such that you can heve several trust levels to restrict what other `radau` can archive.
The default must be "untrusted", such that - if anything fails - no harm can be done.

Only trusted other `radau` then can do things which are explicitly allowed.

This should be safe in that there is no general trust, but clear trust groups which all have a clear way to specify things.
There also must be protection that something does not accidentally uses the wrong trust group.

This can be implemented on top of cryptography:

- Any `radau` instance is added to a trust group based on its key properties
- The trust group then defines what this `radau` can archive and do
- If the key changes, the instance automatcally loses access to the trust group

Perhaps this is not enough.  Perhaps the trust group should certify the key and only certified keys then are allowed.
This means, you an instance cannot be accidentally be added to a groups not knowing that it was added to the group.
It must also cooperate, hence there must be a working backchannel.  Without this, adding to a group fails.

So the sending party not only needs its key, it also needs to include the right certificate for this.


## Patterns

Patterns:

- DRY: Don't Repeat Yourself
- KISS: Keep It Stupid Simple
- ASAP: As Short As Possible and As Soon As Possible
- Perfection is reached if you can no more leave anything away.
- Free: As in free beer, free speech, free baby
- Kaizen: Positive evolution
- If in doubt, stay human and just do the right thing
  - Keep in mind: To be human is an antipattern of evolution.

Antipatterns:

- Slavery.  This includes:
  - Intellectual Property
  - Copyright
  - Code of Conduct
  - Political Correctness
- Hate.  This includes:
  - Anger
  - Censoring
  - Envy
  - Intolerance
  - Hate Speech
  - Nazis
- Fraud.  This includes:
  - Lies
  - Fake News
  - Untruthiness
  - Crime
- Force.  This includes:
  - Despotism
  - Repression
  - Violence
  - Weapons
  - War


## FAQ

T.B.D.

