from conans import ConanFile


class PromisedFutureConan(ConanFile):
	name = "PromisedFuture"
	version = "1.3"
	license = "MIT license"
	url = "https://github.com/michaelstein/promised-future"
	author = "Michael Stein"
	description = "A JavaScript-style Promise for C++20"
	homepage = "https://github.com/michaelstein/promised-future"
	no_copy_source = True
	exports_sources = "src/*", "LICENSE"
	
	def package(self):
		self.copy("LICENSE", "licenses")
		self.copy("*.h", dst="include", src="src")
	
	def package_id(self):
		self.info.clear()
