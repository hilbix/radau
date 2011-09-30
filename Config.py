#!/usr/bin/env python

from Logging import Log
import Reg
import Target
import Port
import Tool
import Defaults
import socket

class Config:
	"""
	Represent the configuration
	"""
	cfg = {}

	def __init__(self, name):
		self.name = name
		self.lineno = 0
		self.target = Target.List()
		self.port = Port.List()

	def __setattr__(self,name,val):
		self.cfg[name] = val

	def __getattr__(self,name):
		try:
			return self.cfg[name]
		except KeyError:
			raise AttributeError, name

class Load(Config):
	def __init__(self, name):
		Config.__init__(self, name)

	def load_line(self, line):
		self.lineno += 1
		s = line.strip()
		if s == "" or s[0] == "#":
			return
		s = Reg.spaces.split(s)
		try:
			fn = getattr(self,'cfg_'+s[0])
		except AttributeError as e:
			Log.xerr('config syntax: {0}:{1}: {3} {4}: {2}', self.name, self.lineno, line, type(e), e)
			return
		try:
			fn(*s[1:])
		except Exception as e:
			Log.xerr('config error: {0}:{1}: {3} {4}: {2}', self.name, self.lineno, line, type(e), e)
			return

	def cfg_listen6(self, nameport):
		np = Tool.hostport(nameport, Defaults.PORT)
		self.port.add(socket.AF_INET6, np)

	def cfg_listen4(self, nameport):
		np = Tool.hostport(nameport, Defaults.PORT)
		self.port.add(socket.AF_INET, np)

	def cfg_host(self, nameport, *args):
		np   = Tool.hostport(nameport, Defaults.PORT)
		name = np[0]
		port = len(np)>1 and Tool.intval(np[1]) or _PORT
		pw   = len(np)>2 and np[2] or ''
		sa   = socket.getaddrinfo(name, port)
		if not sa:
			Log.err("cannot resolve %s",name)
			return

		for a in sa:
			if a[0] != socket.AF_INET and a[0] != socket.AF_INET6:
				Log.warn("wrong family {0} for {1}", Tool.sockfam(a[0]), name)
				continue
			if a[1] != socket.SOCK_DGRAM:
				Log.info("ignore type {0} for {1}", Tool.socktype(a[1]), name)
				continue
			if a[2] != socket.IPPROTO_UDP:
				Log.info("ignore proto {0} for {1}", Tool.sockproto(a[2]), name)
				continue
			Log.info("host {0}: {2} {1}", name, a[4], Tool.sockfam(a[0]))
			self.target.add(a[0], a[4], pw)

class Loader(Load):
	"""
	Load the configuration
	"""
	def __init__(self, name):
		Load.__init__(self, name)
		self.load()

	def load(self, name=None):
		cfg = Load(name or self.name)
		with open(cfg.name, 'r') as f:
			for line in f:
				cfg.load_line(line)
			for attr in dict(cfg.cfg):
				self.cfg[attr] = cfg.cfg[attr]

