import setuptools

from pybind11.setup_helpers import Pybind11Extension


ext_modules = [
    Pybind11Extension(
        "mnum",
        ["mnum.cpp"],
        cxx_std=11,
    ),
]


_description = "A mutable number data type that behaves like " + \
    "a number and provides additional mutable properties."


setuptools.setup(
    name="mnum",
    version="1.0",
    author="David Sahani",
    license="GPL-3.0 license",
    description=_description,
    ext_modules=ext_modules,
    install_requires=["pybind11>=2.6.0"],
    packages=setuptools.find_packages(),
    data_files=[('.', ['mnum.pyi'])],
)
