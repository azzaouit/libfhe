# pylibfhe

Python wrapper for [libfhe](https://libfhe.org)

# Dependencies

* [libfhe](https://git.libfhe.org) - See [INSTALL](https://git.libfhe.org/core/libfhe/-/blob/main/INSTALL) for build instructions

* GNU multiprecision libraries

```bash
apt install -y libgmp-dev libmpfr-dev libmpc-dev
```

# Installing (PyPI)

```bash
pip install libfhe
```

# Building from Source

```bash
git clone https://git.libfhe.org/core/libfhe.git
cd libfhe/python
python setup.py install
cd tests && py.test # run test suite
```
