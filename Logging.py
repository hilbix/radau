#

import time
import sys
import traceback

class Log:

	@classmethod
	def out2(klass, prefix, str):
		print "{0.tm_year}{0.tm_hour:02}{0.tm_sec:02}-{0.tm_hour:02}{0.tm_min:02}{0.tm_sec:02} {1}: {2}".format(time.gmtime(), prefix, str)

	@classmethod
	def out(klass, prefix, log, args):
		klass.out2(prefix, log.format(*args))

	@classmethod
	def err(klass, log, *args):
		klass.out("error", log, args)

	@classmethod
	def xerr(klass, log, *args):
		klass.out("error", log, args)
		exc = sys.exc_info()
		if exc:
			for s in traceback.format_exception(*exc):
				klass.out2("E", s.rstrip('\n'))

	@classmethod
	def warn(klass, log, *args):
		klass.out("warning", log, args)

	@classmethod
	def info(klass, log, *args):
		klass.out("info", log, args)

