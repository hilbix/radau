#

import time

class Log:

	@classmethod
	def out(klass, prefix, log, args):
		print "{0.tm_year}{0.tm_hour:02}{0.tm_sec:02}-{0.tm_hour:02}{0.tm_min:02}{0.tm_sec:02} {1}: {2}".format(time.gmtime(), prefix, log.format(*args))

	@classmethod
	def err(klass, log, *args):
		klass.out("error", log, args)

	@classmethod
	def warn(klass, log, *args):
		klass.out("warning", log, args)

	@classmethod
	def info(klass, log, *args):
		klass.out("info", log, args)

