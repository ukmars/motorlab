#!/usr/bin/env python3
# -*- coding:utf-8 -*-
###
# Project: <<project>>
# File:    first-order-response.py
# File Created: Thursday, 8th December 2022 10:25:57 pm
# Author: Peter Harrison 
# -----
# Last Modified: Friday, 9th December 2022 8:18:01 am
# -----
# Copyright 2022 - 2022 Peter Harrison, Micromouseonline
# -----
# Licence:
# MIT License
# 
# Copyright (c) 2022 Peter Harrison
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is furnished to do
# so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
###

# Adapted From https://www.embeddedrelated.com/showarticle/590.php

import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import lti, lsim, step

def exp(x,Km,Tm):
    if x > 0:
        y = Km*(1-np.exp(-x/Tm))
    else:
        y = 0
    return y
       

plt.figure(figsize=(8, 6))
Km = 200
Tm = 0.5
t_start = -0.5
t_end = 3.0
t = np.arange(t_start, t_end, 0.001)
y = [exp(x,Km,Tm) for x in t]
plt.plot(t, (t > 0)*Km) 
plt.plot(t, y)
plt.plot([0, Tm, Tm], [0, Km, 0], '-.')
plt.xlim([t_start, t_end])
plt.ylim([0, 250])
plt.grid('on')
plt.xlabel(r'time(s)', fontsize=16)
plt.title(F'First order unit step response with time constant. Km = {Km},  Tm={Tm}',
          fontsize=12)
plt.annotate('Slope = Km/Tm', xytext=(-0.2,220), xy = (0.4,170),  arrowprops=dict(arrowstyle="->", shrinkB=5))
plt.annotate('1 * Tm', xytext=(1 * Tm + Tm/8,15), xy = (1 * Tm,0),  arrowprops=dict(arrowstyle="->", shrinkB=5))
plt.annotate('2 * Tm', xytext=(2 * Tm + Tm/8,15), xy = (2 * Tm,0),  arrowprops=dict(arrowstyle="->", shrinkB=5))
plt.annotate('3 * Tm', xytext=(3 * Tm + Tm/8,15), xy = (3 * Tm,0),  arrowprops=dict(arrowstyle="->", shrinkB=5))
plt.annotate('4 * Tm', xytext=(4 * Tm + Tm/8,15), xy = (4 * Tm,0),  arrowprops=dict(arrowstyle="->", shrinkB=5))
plt.annotate('5 * Tm', xytext=(5 * Tm + Tm/8,15), xy = (5 * Tm,0),  arrowprops=dict(arrowstyle="->", shrinkB=5))
plt.annotate('63%', xytext=(1*Tm+Tm/8,exp(1*Tm,Km,Tm)-Km/8), xy = (1*Tm,exp(1*Tm,Km,Tm)), arrowprops=dict(arrowstyle="->", shrinkB=5))
plt.annotate('86%', xytext=(2*Tm+Tm/8,exp(2*Tm,Km,Tm)-Km/8), xy = (2*Tm,exp(2*Tm,Km,Tm)), arrowprops=dict(arrowstyle="->", shrinkB=5))
plt.annotate('95%', xytext=(3*Tm+Tm/8,exp(3*Tm,Km,Tm)-Km/8), xy = (3*Tm,exp(3*Tm,Km,Tm)), arrowprops=dict(arrowstyle="->", shrinkB=5))
plt.annotate('98%', xytext=(4*Tm+Tm/8,exp(4*Tm,Km,Tm)-Km/8), xy = (4*Tm,exp(4*Tm,Km,Tm)), arrowprops=dict(arrowstyle="->", shrinkB=5))
plt.annotate('99%', xytext=(5*Tm+Tm/8,exp(5*Tm,Km,Tm)-Km/8), xy = (5*Tm,exp(5*Tm,Km,Tm)), arrowprops=dict(arrowstyle="->", shrinkB=5))
plt.show()
