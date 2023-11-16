#!/usr/bin/python3
import random

def binSearch(v, x):
    l = 0
    h = len(v)-1
    m = h//2

    if h<0: return 0

    while True:
        if m >= len(v): break
        if m < 0: break
        if v[m] < x:
            l = m + 1
            m = l + (h - l) // 2
        elif v[m] > x:
            h = m - 1
            m = l + (h - l) // 2
        else:
            break
        if h - l <=1:
            break
        
    while l<len(v) and v[l] < x:
        l += 1
    return l
while True:
    l = []

    for i in range(random.randint(2,100)):
        s = random.random()*100
        l.insert(binSearch(l,s), s)

    j = min(l)
    for i in l:
        assert(i>=j)
        j = i