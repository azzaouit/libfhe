# pylibfhe

Python wrapper for [libfhe](https://github.com/azzaouit/libfhe)

# Dependencies

* [libfhe](https://github.com/azzaouit/libfhe) - See [INSTALL](https://github.com/azzaouit/libfhe/blob/master/INSTALL) for build instructions

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
git clone https://github.com/azzaouit/libfhe.git
cd libfhe/python
python -m venv venv
source venv/bin/activate
python setup.py install
cd tests && py.test
```
