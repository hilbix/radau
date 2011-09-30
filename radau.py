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

class Target:
	"""
	Represent a single target
	"""
	cnt = 0
	def __init__(self, proto, ip, pw):
		self.proto = proto
		self.ip    = ip
		self.pw    = pw

class TargetList:
	"""
	Manage a target list
	"""
	dest = []

	def add(self, proto, ip, pw):
		host = Target(proto, ip, pw)
		self.dest.append(host)

	def __len__(self):
		return len(self.dest)

	def __getitem__(self,x):
		return self.dest[x]

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
	Server(cfg, Socket.AF_INET)
	Server(cfg, Socket.AF_INET6)
	asyncore.loop(1)

