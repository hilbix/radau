Rants about why there isn't a proper thing out yet.

My idea of a proper coroutine API must fulfill following coding style:

```python
@co_with_single_return_value
def _get_one_url(url):
  return (url, urllib2.urlopen(url).read())

@co_ok
def get_urls(urls):
  results = []
  sched = Scheduler()
  for url in urls:
    sched.add(_get_one_url, url)
  return sched

# Use following syntax to wait for the list first:
#for (url,res) in get_urls(['http://google.com/','http://example.com/']).wait():
for (url,res) in get_urls(['http://google.com/','http://example.com/']):
  print url,res
```

Notes:

* There may be other coding styles supported, but this is the only proper coding style I will accept to use, ever.  Everything else sucks.

* Decorators are OK.  They even will(!) improve the code quality, as they explain things, for example that _get_one_url is a coroutine which spits out a single result.

* "Superfluous" decorators like `co_ok` are nice, too.  This aids debugging in the case some non-coroutine-enabled version of something accidentally is called.

* The call to `Scheduler()` groups the different flags, such that the call to `.add()` (or whatevere it's called) can be kept clean for passing all args and kwrds to the coro.

* If you need different flags, use a special purpose method - or chained Scheduler objects.  I would even suggest that `sched.add` is clumsy as well, why not keep `sched` callable like in prototyped languages?

Note that instead of `sched.add()` I favor just to call `sched()`.  However there should be some methods on `sched` as well to allow things like `sched.wait()`


Greenhouse
----------
https://github.com/teepark/greenhouse/

Because I want to keep my mental health I definitively never will be able to type in is something like:
`greenhouse.schedule(_get_one_url, args=(url, results))`

Right, you can correct this with simple code like this:
```
def ghsched(fn, *args):
   greenhouse.schedule(fn, args=args)
```
but as it is this easy, why isn't this supported by default?

Also I really hate to use "global" variants of coding.  There may be used some globals behind the scenes, but I never want to write code, which relies on such a paradigma.  So `greenhouse.schedule` is not a function I will ever use.  This, again, can be corrected, of course like with following:
```
def NewSched(*Cargs, **Ckwrds):
  def add(fn, *args):
    greenhouse.schedule(fn, *Cargs, **Ckwrds, args=args)
  add.add = add
  return add
```

And with even more effort you can probably tweak it to support the suggested coding style, too.

The nice thing about greenhouse is, that it provides monkey patched versions of nearly everything you ever needed.  This allows to write your code natuarlly and then use it as a coroutine.

Well done here.


Cogen
-----
http://code.google.com/p/cogen/

Most said to Greenhouse (which apparently tries to stay lookalike to Gogen) applies to Cogen as well (it has no support for monkey patching, though).  But one additional thing must be mentioned:

Cogen needs `yield` as it is based on Python's coroutines.  This is not very coding firendly.

An ideal coroutine library would enable you to transform the single threaded blocking way into a parallel application with following steps:

- Separate function blocks into proper separated methods.  This should have been done anyway already, as monolithic spaghetti coding style is unreadable anyway.

- Apply decorators to the now smaller functions, describing their behavior.

- Alter all factories of blocking handles, to use the nonblocking handle creation now.

- In case of Pure Python it probably means to transform everything into a yield.  This perhaps even can be done semi-automatic, as every evaluation in every function which is a coroutine then can use the yield case, and the few cases where yield is not needed then can be spit out on runtime to stderr (or ignored with some minor performance penalty).

The latter perhaps is unclear, but if the scheduler knows to spit out a warning and just yield back any value it does not grok, you are even able to write `a = yield (yield 1) + (yield 1)` if you like and it will work as expected in case you must replace `1` with some variable passed in, and the variables might block to yield a result.

You think this is nuts?  That's because Python builtin coroutines are not what I would expect them to be.  They cannot yield in function calls.  Either you introduce a stack or you have to live with this performance penalty until there is an optimizer which is able to remove this unneccessary yielding on runtime.

I dislike to write yield everywhere.  This is why I do not think about using cogen.  This also probably means, that my code will not run on embedded versions of Python, like jython or stackless.

However there is cogen.magic today, which enables the use of greenlets in cogen.  This does a nice job on removing most yields in favor of a decorator.

Well done!  Even that the cogen API still is not what I really want to see, it is then very similar to greenhouse.

Cogen has the opportunity that you then can do it as you like:

- The way offered by greenhouse using greenlets - note that cogen could support monkey patching like greenhouse does
- The way offered by cogen
- Support for Python WSGI middleware
- The standard Python coroutine way

etc.  So if you consider using greenhouse, perhaps look if cogen.magic fits your needs.


Kamaelia
--------
https://www.kamaelia.org/

This has the unique interesting property of being 1.x, whatever this wants to tell, as cogen and greenhouse are quite usable (at least what I read about it).

Kamaelia has the often observed boring and tedious property of the documentation being a "teaching" document instead of a reference manual.  So it is very confusing and hard to read and not a good way to start with it.

For my part I like to have a document starting at the point where it gets interesting.  No babble about what coroutines are, or how ingenious the inventers were (like in the Exim4 documentation) or why things are done this way.

Nope.  Get to the point!  Do the common and hard stuff first, then explain the rest based on that example.

Take this document as example.  I write, what I want to see, then I discuss the rest.

Kamaelia - like Twisted - is not lightweight.  It offers a strict framework in which you must add your code.

This might be good for some, but is not for me.  I like to just start coding, and then sort things out later.  Just get to the point, then add all the nice things.

If I need to adopt to a framework first, I probably never start coding, because before I then come to the point to start to write a single line, I am already busy doing other things.

That is why Python with batteries included is so quick to use.  Just dive into coding.  No big environment to use.  Nothing to think about.  Pure code.

Using a framework which takes away this freedom is nothing for me.

Perhaps I missed that Kamaelia can adopt to my style.  But I did not find it in the documentation in the first place.  If it is hidden too deeply, I must say that this is not a failure on my side then.

