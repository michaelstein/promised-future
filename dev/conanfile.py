from conans import ConanFile
from conan.tools.cmake import CMake, CMakeDeps


class PromisedFutureDevConan(ConanFile):
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
