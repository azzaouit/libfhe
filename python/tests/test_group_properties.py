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

from fhe import *
import numpy as np

lgd = 14
lgq = 237
lgm = 60
t = 65537

b = BGV(lgd, lgq, lgm, t)
k = b.keygen()

def test_symmetry():
    x1 = np.random.randint(t, size=1<<lgd, dtype=np.uint64)
    x = k.encrypt(x1)
    x_dec = x.decrypt(k.secret, t)
    assert (x_dec == x1).all()

def test_add_commutative():
    x1 = np.random.randint(t, size=1<<lgd, dtype=np.uint64)
    x2 = np.random.randint(t, size=1<<lgd, dtype=np.uint64)
    modsum = np.remainder(x1 + x2, t)

    x = k.encrypt(x1)
    y = k.encrypt(x2)

    xy = x+y
    xy_dec = xy.decrypt(k.secret, t)

    yx = y+x
    yx_dec = yx.decrypt(k.secret, t)

    assert (xy_dec == yx_dec).all()
    assert (xy_dec == modsum).all()

def test_mul_commutative():
    x1 = np.random.randint(t, size=1<<lgd, dtype=np.uint64)
    x2 = np.random.randint(t, size=1<<lgd, dtype=np.uint64)

    x = k.encrypt(x1)
    y = k.encrypt(x2)

    xy = x*y
    xy_dec = xy.decrypt(k.secret, t)

    yx = y*x
    yx_dec = yx.decrypt(k.secret, t)

    assert (xy_dec == yx_dec).all()

def test_distributive():
    x1 = np.random.randint(t, size=1<<lgd, dtype=np.uint64)
    x2 = np.random.randint(t, size=1<<lgd, dtype=np.uint64)
    x3 = np.random.randint(t, size=1<<lgd, dtype=np.uint64)

    x = k.encrypt(x1)
    y = k.encrypt(x2)
    z = k.encrypt(x3)

    lhs = x*(y+z)
    lhs_dec = lhs.decrypt(k.secret, t)

    rhs = x*y + x*z
    rhs_dec = rhs.decrypt(k.secret, t)

    assert (lhs_dec == rhs_dec).all()
