# pylibfhe
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import os
import numpy as np
from setuptools import find_packages, Extension, setup
from Cython.Build import cythonize

FHE_INCLUDE_DIR = os.path.abspath(os.path.join("..", "include"))

ext_modules = [
    Extension(
        "fhe",
        ["fhe/fhe.pyx"],
        libraries=["fhe"],
        include_dirs=[FHE_INCLUDE_DIR, np.get_include()],
    )
]

with open("README.md") as f:
    long_description = f.read()

with open("requirements.txt", "r") as fh:
    reqs = fh.readlines()

setup(
    name="libfhe",
    version="0.0.1",
    ext_modules=cythonize(ext_modules),
    author="azzaoui",
    author_email="taha@azzaoui.org",
    description="Python wrapper for libfhe",
    long_description=long_description,
    long_description_content_type="text/markdown",
    setup_requires=["wheel"],
    include_package_data=True,
    install_requires=reqs,
    url="https://libfhe.org",
    packages=find_packages(),
    keywords = ["cryptography", "fhe", "Homomorphic Encryption"],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
		"Topic :: Security :: Cryptography",
        "Operating System :: Unix",
        "Operating System :: MacOS",
    ],
    python_requires=">=3.11"
)
