---
title: Quaternion Algebra Implementations
author: Alli Nilles
execute: quaternions.py quaternions.hs
...


Python Implementation
=====================

```python{exec:quaternions.py}

import math
import numpy as np

def mkq(theta, vx, vy, vz, eps=0.0001):
    a = abs(math.cos(theta/2.0))
    b = vx*math.sin(theta/2.0)
    c = vy*math.sin(theta/2.0)
    d = vz*math.sin(theta/2.0)

    norm = math.sqrt(a*a + b*b + c*c + d*d)
    a = a/norm
    b = b/norm
    c = c/norm
    d = d/norm

    # make representation unique
    if a <= eps:
        a = 0
        b = abs(b)
        if b <= eps:
            b = 0
            c = abs(c)
            if c <= eps:
                c = 0
                d = 1

    return (a,b,c,d)

def conj(q):
    return (q[0], -q[1], -q[2], -q[3])

def dot3(v1, v2):
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]

def cross3(v1, v2):
    return np.cross(np.array(v1), np.array(v2)).tolist()

def qmult(q1, q2):
    i1 = q1[1:]
    i2 = q2[1:]
    [x1, x2, x3] = cross3(i1, i2)
    a = q1[0]*q2[0] - dot3(i1, i2)
    b = q1[0]*q2[1] + q1[1]*q2[0] + x1
    c = q1[0]*q2[2] + q1[2]*q2[0] + x2
    d = q1[0]*q2[3] + q1[3]*q2[0] + x3
    return (a,b,c,d)

```

Haskell Implementation
======================

```haskell{exec:quaternions.hs}

type Quat = (Double, Double, Double, Double)

qadd :: Quat -> Quat -> Quat
qadd (a1, b1, c1, d1) (a2, b2, c2, d2) = (a1+a2, b1+b2, c1+c2, d1+d2)

qsub :: Quat -> Quat -> Quat
qsub (a1, b1, c1, d1) (a2, b2, c2, d2) = (a1-a2, b1-b2, c1-c2, d1-d2)

qconj :: Quat -> Quat
qconj (a1, b1, c1, d1) = (a1, -b1, -c1, -d1)

-- 3 tuple operations only
dot3 :: Fractional a => (a,a,a) -> (a,a,a) -> a
dot3 (a1,b1,c1) (a2,b2,c2) = a1*a2 + b1*b2 + c1*c2

cross3 :: Fractional a => (a,a,a) -> (a,a,a) -> (a,a,a)
cross3 (b1,c1,d1) (b2,c2,d2) = (c1*d2 - d1*c2, d1*b2 - b1*d2, b1*c2 - c1*b2)

qmult :: Fractional a => Quat -> Quat -> Quat
qmult h1@(a1, b1, c1, d1) h2@(a2, b2, c2, d2) = (a3, b3, c3, d3)
  where   i1 = (b1, c1, d1)
          i2 = (b2, c2, d2)
          (x1, x2, x3) = cross3 i1 i2
          a3 = a1*a2 - dot3 i1 i2
          b3 = a1*b2 + a2*b1 + x1
          c3 = a1*c2 + a2*c1 + x2
          d3 = a1*d2 + a2*d1 + x3

```
