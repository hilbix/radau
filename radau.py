#!/usr/bin/env python

import asyncore
import socket
import time
import sys
import re

from Logging import Log
import Tool
import Config

# Radau -> 0x4adau -> Unsigned 0x4ada -> 19162
_PORT = 0x4ada
_CFGFILE = "radau.cfg"

class Packet:
	pass

class Server(asyncore.dispatcher):

	cfg  = None
	proto = None
	last = 0
	dpos = 0

	def __init__(self, cfg, n):
		self.cfg = cfg
		asyncore.dispatcher.__init__(self)

		self.create_socket(cfg.port[n].proto, socket.SOCK_DGRAM)
		self.bind((self.addr or "", self.port))
		self.set_reuse_addr()

	def handle_connect(self):
		print "connect"

	def handle_read(self):
		data, addr = self.recvfrom(1012)
		print addr, " = ", data

	def handle_write(self):
		try:
			host = self.cfg.dest[ self.dpos % len(self.cfg.dest) ]
			print "write", self.dpos, repr(adr)
			self.sendto("x", adr)
		except:
			print "write", self.dpos, repr(sys.exc_info())
		self.dpos += 1

	# Every second we send a packet
	def writable(self):
		was = self.last
		self.last = int(time.time())
		return self.last!=was

if __name__ == '__main__':
	cfg = Config.Loader(_CFGFILE)
	Server(cfg, socket.AF_INET)
	Server(cfg, socket.AF_INET6)
	asyncore.loop(1)

