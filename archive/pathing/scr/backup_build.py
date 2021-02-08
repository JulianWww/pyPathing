from setuptools import setup, find_packages, Extension
from Cython.Build import cythonize
import time
from os.path import join, dirname

extensions = [Extension("cy_generators", [join(dirname(__file__), "cy_generators.pyx")])]

setup(
    ext_modules=cythonize(extensions, annotate=True)
)