from conans import ConanFile, tools
from conan.tools.cmake import CMake, CMakeDeps
import os


class PromisedFutureTestConan(ConanFile):
	settings = "os", "compiler", "build_type", "arch"
	generators = "CMakeDeps", "CMakeToolchain", "virtualenv", "virtualrunenv", "virtualbuildenv"
	
	tool_requires = (
		"cmake/3.22.3",
		"gtest/1.12.1"
	)
	
	def generate(self):
		cmake = CMakeDeps(self)
		cmake.build_context_activated = ["gtest"]
		cmake.generate()

	def build(self):
		cmake = CMake(self)
		cmake.configure()
		cmake.build()

	def imports(self):
		self.copy("*.dll", dst="bin", src="bin")
		self.copy("*.dylib*", dst="bin", src="lib")
		self.copy('*.so*', dst='bin', src='lib')

	def test(self):
		if not tools.cross_building(self):
			os.chdir("bin")
			self.run(".%sPromisedFuture_Test" % os.sep)
