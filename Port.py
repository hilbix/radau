#

class Port:
	"""
	Represent a local listening port
	"""
	def __init__(self, proto, ip):
		self.proto = proto
		self.ip    = ip

from Lists import MTDL
import Defaults

class List(MTDL):
	def __init__(self):
		MTDL.__init__(self, Port)
		self.default = Port(Defaults.PROTO, Defaults.IP)

