#

class MTDL:
	"""
	Managed Typed Default List

	This is a list with a special default property
	in case it is empty.
	"""
	list = []
	default = None
	
	def __init__(self, type):
		self.type = type

	def add(self, *args):
		ent = self.type(*args)
		self.list.append(ent)

	def __len__(self):
		return self.list and len(self.list) or self.default and 1 or 0

	def __getitem__(self,x):
		return not self.list and self.default or self.list[x]

