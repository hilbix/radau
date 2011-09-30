#
# Why are there no standard functions for this already?
# These are very common things.

import socket

def intval(s,d=0):
	"""
	return string converted to integer ignoring errors.
	Or the given default value if this fails.
	"""
	try:
		return int(s)
	except:
		return d

def fn_startswith(prefix):
	"""returns a closure which returns true when the passed string matches prefix"""
	return lambda s: s.startswith(prefix)

def hostport(s,port=0):
	"""
	Return a suitable (host,port) tuple.
	Accepts all usual string combinations.
	Port is optional if a default is given, else host is optional.
	The default host is "" which can be aliased as "*"
	"""
	def ret(hp):
		h = hp[0]
		p = hp[1]
		if h and h[0]=='[' and h[-1]==']':
			h = h[1:-1]
		return (h!='*' and h or '', intval(p) or socket.getservbyname(p))

	hp = s.rsplit(':',1)
	if s and s[0]=='[':
		# we have a host part
		if len(hp)==2 and hp[0][-1]==']':
			return ret(hp)
		return ret((s,port))

	if len(hp)==2:
		return ret(hp)
	if port:
		return ret((s,port))

	return ret(('',s))

def rev(d):
	"""reverse a dict: {a:b} becomes {b:a}"""
	return dict((v,k) for (k,v) in d.iteritems())

def _sockdictgetter(prefix, err):
	"""Create a mapping function from IDs to socket  socket constants to a dict"""
	vals = dict((getattr(socket,x),x) for x in filter(fn_startswith(prefix), dir(socket)))
	def getter(key):
		return vals.get(key, 'unknown sock {0} {1}'.format(err, key))
	return getter

sockfam   = _sockdictgetter('AF_',      'family')
socktype  = _sockdictgetter('SOCK_',    'type')
sockproto = _sockdictgetter('IPPROTO_', 'proto')

