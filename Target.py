#

class Target:
	"""
	Represent a single target
	"""
	cnt = 0
	def __init__(self, proto, ip, pw=''):
		self.proto = proto
		self.ip    = ip
		self.pw    = pw

from Lists import MTDL
import Defaults

class List(MTDL):
	def __init__(self):
		MTDL.__init__(self, Target)
		self.default = Target(Defaults.PROTO, Defaults.IP)

