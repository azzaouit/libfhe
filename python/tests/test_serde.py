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

def test_key():
    buf = k.bytes()
    k1 = b.key()
    k1.from_bytes(buf)
    assert k == k1

def test_ciphertext():
    x1 = np.random.randint(t, size=1<<lgd, dtype=np.uint64)
    ct1 = k.encrypt(x1)
    buf = ct1.bytes()
    ct2 = k.ciphertext()
    ct2.from_bytes(buf)
    assert ct1 == ct2
