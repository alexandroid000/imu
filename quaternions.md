---
title: Quaternion Algebra Implementations
author: Alli Nilles
execute: quaternions.py Quaternions.hs
...


Python Implementation
=====================

```python{exec:quaternions.py}

import math
from numpy import cross, array

def mkq(theta, vx, vy, vz):

    [a,vx,vy,vz] = normalize([0,vx,vy,vz])

    a = math.cos(theta/2.0)
    b = vx*math.sin(theta/2.0)
    c = vy*math.sin(theta/2.0)
    d = vz*math.sin(theta/2.0)

    return normalize([a,b,c,d])

# make representation unique by restricting to upper half of unit quaternions
def mk_unique(q, eps=2.107342e-08):

    [a,b,c,d] = q

    a = abs(a)
    if abs(a) <= eps:
        a = 0.0
        b = abs(b)
        if abs(b) <= eps:
            b = 0.0
            c = abs(c)
            if abs(c) <= eps:
                c = 0.0
                d = 1.0
    return [a,b,c,d]

# see http://stackoverflow.com/questions/11667783/quaternion-and-normalization/12934750#12934750
def normalize(q, eps=2.107342e-08):

    [a,b,c,d] = q

    norm_sq = a*a + b*b + c*c + d*d

    if abs(1.0 - norm_sq) < eps:
        norm = 2.0 / (1.0 + norm_sq)
    else:
        norm = math.sqrt(norm_sq)

    if (norm != 0.0):
        a = a/norm
        b = b/norm
        c = c/norm
        d = d/norm

    return [a,b,c,d]

def conj(q):
    return (q[0], -q[1], -q[2], -q[3])

def dot3(v1, v2):
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]

def cross3(v1, v2):
    return cross(array(v1), array(v2)).tolist()

def rot_vector(q, v):

    new_q = qmult(q, qmult(v, conj(q)))

    return normalize(new_q)

def qmult(q1, q2):
    i1 = q1[1:]
    i2 = q2[1:]
    [x1, x2, x3] = cross3(i1, i2)
    a = q1[0]*q2[0] - dot3(i1, i2)
    b = q1[0]*q2[1] + q1[1]*q2[0] + x1
    c = q1[0]*q2[2] + q1[2]*q2[0] + x2
    d = q1[0]*q2[3] + q1[3]*q2[0] + x3
    return [a,b,c,d]

```

Haskell Implementation
======================

```haskell{exec:Quaternions.hs}

module Quaternions where

type Quat = (Double, Double, Double, Double)

mkq :: Double -> Double -> Double -> Double -> Quat
mkq theta vx vy vz = normalize (a,b,c,d)
    where   vnorm   = sqrt (vx*vx + vy*vy + vz*vz)
            a       = cos (theta/2.0)
            b       = vx * sin (theta/2.0) / vnorm
            c       = vy * sin (theta/2.0) / vnorm
            d       = vz * sin (theta/2.0) / vnorm

normalize :: Quat -> Quat
normalize q@(a, b, c, d) = (a/nm, b/nm, c/nm, d/nm)
    where nm = norm q

norm :: Quat -> Double
norm (a, b, c, d)
    | norm_sq == 0.0 = 1
    | abs (1 - norm_sq) < eps = 2.0 / (1.0 + norm_sq)
    | otherwise = sqrt norm_sq
    where   eps     = 2.107342e-08
            norm_sq = a*a + b*b + c*c + d*d


qadd :: Quat -> Quat -> Quat
qadd (a1, b1, c1, d1) (a2, b2, c2, d2) = (a1+a2, b1+b2, c1+c2, d1+d2)

qsub :: Quat -> Quat -> Quat
qsub (a1, b1, c1, d1) (a2, b2, c2, d2) = (a1-a2, b1-b2, c1-c2, d1-d2)

qconj :: Quat -> Quat
qconj (a1, b1, c1, d1) = (a1, -b1, -c1, -d1)

-- 3 tuple operations only
dot3 :: Fractional a => (a,a,a) -> (a,a,a) -> a
dot3 (a1,b1,c1) (a2,b2,c2) = a1*a2 + b1*b2 + c1*c2

-- 3 tuple operations only
dot2 :: Fractional a => (a,a) -> (a,a) -> a
dot2 (a1,b1) (a2,b2) = a1*a2 + b1*b2

cross3 :: Fractional a => (a,a,a) -> (a,a,a) -> (a,a,a)
cross3 (b1,c1,d1) (b2,c2,d2) = (c1*d2 - d1*c2, d1*b2 - b1*d2, b1*c2 - c1*b2)

rotate :: Quat -> Quat -> Quat
rotate q v = qmult q v_qconj
    where   v_qconj = qmult v (qconj q)

qmult :: Quat -> Quat -> Quat
qmult h1@(a1, b1, c1, d1) h2@(a2, b2, c2, d2) = (a3, b3, c3, d3)
    where   i1 = (b1, c1, d1)
            i2 = (b2, c2, d2)
            (x1, x2, x3) = cross3 i1 i2
            a3 = a1*a2 - dot3 i1 i2
            b3 = a1*b2 + a2*b1 + x1
            c3 = a1*c2 + a2*c1 + x2
            d3 = a1*d2 + a2*d1 + x3

```
