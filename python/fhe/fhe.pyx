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

from libc.stdlib cimport malloc, free
from libc.stdint cimport uintptr_t

import numpy as np
from numpy.ctypeslib import as_array
cimport numpy as cnp

from cpython cimport array
import array
from fhe cimport *

import_gmpy2()

cdef class Ring:
    cdef ring_t *_ptr
    cdef bint ptr_owner

    def __cinit__(self):
        self.ptr_owner = False

    def __dealloc__(self):
        # De-allocate if not null and flag is set
        if self._ptr is not NULL and self.ptr_owner is True:
            ring_free(self._ptr)
            self._ptr = NULL

    def __init__(self, lgd=14, lgq=237, lgm=25):
        ring_init(self._ptr, lgd, lgq, lgm)
        if self._ptr is NULL:
            raise MemoryError
        self.ptr_owner = True

    @property
    def d(self):
        return self._ptr.d

    @property
    def n(self):
        return self._ptr.n

    @property
    def m(self):
        return as_array(<uint64_t[:self.n]>self._ptr.m)

    @property
    def invms(self):
        return as_array(<uint64_t[:self.n]>self._ptr.invms)

    @property
    def roots(self):
        return as_array(<uint64_t[:self.n*self.d]>self._ptr.roots)

    @property
    def iroots(self):
        return as_array(<uint64_t[:self.n*self.d]>self._ptr.iroots)

    @staticmethod
    cdef Ring from_ptr(ring_t *_ptr, bint owner=False):
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef Ring ring = Ring.__new__(Ring)
        ring._ptr = _ptr
        ring.ptr_owner = owner
        return ring

cdef class Poly:
    cdef poly_t *_ptr
    cdef bint ptr_owner

    def __cinit__(self):
        self.ptr_owner = False

    def __dealloc__(self):
        # De-allocate if not null and flag is set
        if self._ptr is not NULL and self.ptr_owner is True:
            poly_free(self._ptr)
            self._ptr = NULL

    def __init__(self):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __repr__(self):
        return str(self.b)

    def __ptr__(self):
        return int(<uintptr_t>self._ptr)

    @property
    def b(self):
        return as_array(<int64_t[:self.n*self.d]>self._ptr.b)

    @property
    def r(self):
        return Ring.from_ptr(self._ptr.r, False)

    @property
    def n(self):
        return int(self._ptr.r.n)

    @property
    def d(self):
        return int(self._ptr.r.d)

    def __len__(self):
        return self.d * self.n

    @property
    def is_ntt(self):
        return self._ptr.is_ntt

    def decode(self, modulus):
        d = int(self._ptr.r.d)
        cdef cnp.uint64_t[:] a = np.zeros(d, dtype=np.uint64, order="C")
        poly_decode(&a[0], self._ptr, modulus)
        return a

    def ntt(self):
        poly_ntt(self._ptr)

    def intt(self):
        poly_intt(self._ptr)

    @staticmethod
    cdef Poly from_ptr(poly_t *_ptr, bint owner=False):
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef Poly poly = Poly.__new__(Poly)
        poly._ptr = _ptr
        poly.ptr_owner = owner
        return poly

cdef class BGVKey:
    cdef bgv_key_t k
    cdef bgv_t *b

    def __dealloc__(self):
        bgv_key_free(&self.k)

    def __eq__(self, other: BGVKey):
        return bgv_key_cmp(&self.k, &other.k)

    @property
    def secret(self):
        return Poly.from_ptr(&self.k.s, False)

    @property
    def pub(self):
        return (Poly.from_ptr(&self.k.pub.a, False), Poly.from_ptr(&self.k.pub.b, False))

    @property
    def eval(self):
        return (Poly.from_ptr(&self.k.eval.a, False), Poly.from_ptr(&self.k.eval.b, False))

    def encrypt(self, cnp.ndarray[uint64_t, mode="c"] pt not None):
        if len(pt) != int(self.b.r.d):
            raise ValueError("Invalid polynomial length")

        cdef bgv_ct_t *ct =  <bgv_ct_t *>malloc(sizeof(bgv_ct_t))
        if ct is NULL:
            raise MemoryError

        cdef poly_t out
        poly_encode(&self.b.r, &pt[0], &out)

        bgv_encrypt(self.b, ct, &self.k.pub, &out)
        poly_free(&out)

        return CipherText.from_ptr(ct, &self.k.eval, True)

    def ciphertext(self):
        cdef bgv_ct_t *ct =  <bgv_ct_t *>malloc(sizeof(bgv_ct_t))
        if ct is NULL:
            raise MemoryError
        bgv_ct_init(&self.b.r, ct, 2) # Initial ciphertext size always 2
        return CipherText.from_ptr(ct, &self.k.eval, True)

    def bytes(self):
        cdef ring_t* r = <ring_t*>&self.b.r
        buflen = r.d * r.n * 40
        cdef unsigned char* buf = <unsigned char*>malloc(buflen)
        if buf is NULL:
            raise MemoryError
        bgv_key_serialize(buf, &self.k)
        try:
            b_str = buf[:buflen] # Performs a copy of the data
        finally:
            free(buf)
        return b_str

    def from_bytes(self, buf):
        cdef ring_t* r = <ring_t*>&self.b.r
        buflen = r.d * r.n * 40
        if len(buf) != buflen:
            raise ValueError("Invalid buffer size")
        bgv_key_deserialize(r, &self.k, <unsigned char*>buf)

    @staticmethod
    cdef BGVKey keygen(bgv_t *_ptr):
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef BGVKey key = BGVKey.__new__(BGVKey)
        bgv_keygen(_ptr, &key.k)
        key.b = _ptr
        return key

    @staticmethod
    cdef BGVKey key(bgv_t *_ptr):
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef BGVKey k = BGVKey.__new__(BGVKey)
        bgv_key_zero(&_ptr.r, &k.k)
        k.b = _ptr
        return k

cdef class CipherText:
    cdef bgv_ct_t *_ptr
    cdef bgv_keypair_t *_evalptr
    cdef bint ptr_owner

    def __cinit__(self):
        self.ptr_owner = False

    def __dealloc__(self):
        # De-allocate if not null and flag is set
        if self._ptr is not NULL and self.ptr_owner is True:
            bgv_ct_free(self._ptr)
            free(self._ptr)
            self._ptr = NULL

    def __init__(self):
        # Prevent accidental instantiation from normal Python code
        # since we cannot pass a struct pointer into a Python constructor.
        raise TypeError("This class cannot be instantiated directly.")

    def __repr__(self):
        return str(self.c)

    def __add__(self, other):
        return other.__radd__(self)

    def __eq__(self, other: CipherText):
        if self._ptr.n != other._ptr.n:
            return False
        flag = True
        for i in range(self._ptr.n):
            if not poly_cmp(&self._ptr.c[i], &other._ptr.c[i]):
                flag = False
        return flag

    def __radd__(self, other):
        x = <uintptr_t>other.__ptr__()
        cdef bgv_ct_t *o = <bgv_ct_t*>(x)
        cdef bgv_ct_t *out = <bgv_ct_t *>malloc(sizeof(bgv_ct_t))
        if out is NULL:
            raise MemoryError
        bgv_ct_add(out, self._ptr, o)
        return CipherText.from_ptr(out, self._evalptr, True)

    def __mul__(self, other):
        return other.__rmul__(self)

    def __rmul__(self, other):
        x = <uintptr_t>other.__ptr__()
        cdef bgv_ct_t *o = <bgv_ct_t*>(x)
        cdef bgv_ct_t *out = <bgv_ct_t *>malloc(sizeof(bgv_ct_t))
        if out is NULL:
            raise MemoryError
        bgv_ct_mul(out, self._evalptr, self._ptr, o)
        return CipherText.from_ptr(out, self._evalptr, True)

    def __ptr__(self):
        return int(<uintptr_t>self._ptr)

    @property
    def n(self):
        return self._ptr.n

    @property
    def n(self):
        return self._ptr.n

    @property
    def c(self):
        return [Poly.from_ptr(&self._ptr.c[i], False) for i in range(self._ptr.n)]

    def decrypt(self, s, modulus):
        x = <uintptr_t>s.__ptr__()

        cdef poly_t *out = <poly_t *>malloc(sizeof(poly_t))
        if out is NULL:
            raise MemoryError

        cdef poly_t *sk = <poly_t*>x
        bgv_decrypt(out, self._ptr, sk);

        d = self._ptr.c[0].r.d
        cdef cnp.uint64_t[:] a = np.zeros(d, dtype=np.uint64, order="C")
        poly_decode(&a[0], out, modulus)

        free(out)
        return as_array(a)

    def bytes(self):
        cdef ring_t* r = <ring_t*>self._ptr.c.r
        buflen = r.d * r.n * 8 * self._ptr.n
        cdef unsigned char* buf = <unsigned char*>malloc(buflen)
        if buf is NULL:
            raise MemoryError
        bgv_ct_serialize(buf, self._ptr)
        try:
            b_str = buf[:buflen] # Performs a copy of the data
        finally:
            free(buf)
        return b_str

    def from_bytes(self, buf):
        cdef ring_t* r = <ring_t*>self._ptr.c.r
        buflen = r.d * r.n * 8 * self._ptr.n
        if len(buf) != buflen:
            raise ValueError("Invalid buffer size")
        bgv_ct_deserialize(r, self._ptr, <unsigned char*>buf)

    @staticmethod
    cdef CipherText from_ptr(bgv_ct_t *_ptr, bgv_keypair_t * ek, bint owner=False):
        # Fast call to __new__() that bypasses the __init__() constructor.
        cdef CipherText ct = CipherText.__new__(CipherText)
        ct._ptr = _ptr
        ct._evalptr = ek
        ct.ptr_owner = owner
        return ct

cdef class BGV:
    cdef bgv_t b

    def __init__(self, lgd=14, lgq=237, lgm=25, t=65537):
        bgv_init(&self.b, lgd, lgq, lgm, t)

    @property
    def t(self):
        return self.b.t

    @property
    def r(self):
        return Ring.from_ptr(&self.b.r, False)

    def keygen(self):
        return BGVKey.keygen(&self.b)

    def key(self):
        return BGVKey.key(&self.b)

    def __del__(self):
        bgv_free(&self.b)

    def encode(self, cnp.ndarray[uint64_t, mode="c"] p not None):
        cdef poly_t *out =  <poly_t *>malloc(sizeof(poly_t))
        if out is NULL:
            raise MemoryError
        poly_encode(&self.b.r, &p[0], out)
        return Poly.from_ptr(out, True)
