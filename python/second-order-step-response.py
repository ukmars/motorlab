
#!/usr/bin/env python3
# -*- coding:utf-8 -*-
###
# Project: <<project>>
# File:    second--order-step-response.py
# File Created: Thursday, 8th December 2022 10:25:57 pm
# Author: Peter Harrison 
# -----
# Last Modified: Thursday, 8th December 2022 10:44:16 pm
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

# From https://www.embeddedrelated.com/showarticle/671.php

# see also: https://www.javatpoint.com/control-system-time-response-of-second-order-system
 
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import lti

zeta_range = [0.3, 0.5, 0.707, 1.0,  1.5]
omega_range = [2, 3, 5, 10, 20]
wn = 2*np.pi
t = np.arange(0, 4, 0.005)
n = len(t)

fig,plots = plt.subplots(2,sharex = True)
fig.suptitle('Second Order Step Responses',fontsize = 16)
fig.set_size_inches(8,10)


# plt.figure(figsize=(8, 6))
# settling limits
plots[0].plot([-0.5, 4], [0.95, 0.95], '-.', color='skyblue')
plots[0].plot([-0.5, 4], [1.05, 1.05], '-.', color='skyblue')
plots[0].plot([-0.5, 4], [0.98, 0.98], '-.', color='silver')
plots[0].plot([-0.5, 4], [1.02, 1.02], '-.', color='silver')

for k, zeta in enumerate(zeta_range):
    H = lti([wn*wn], [1, 2*zeta*wn, wn**2])
    _, y = H.step(T=t)
    plots[0].plot(t, y, label=F"$\\zeta = {zeta:.3f}$")
    yl = y[58]
    tl = t[58]
    plots[0].annotate(F'$\\zeta={zeta:.3f}$',
                 xy=(tl, yl),
                 xytext=(-0.45, yl),
                 arrowprops=dict(arrowstyle="-", shrinkB=1.5),
                 verticalalignment='center',
                 fontsize=10
                 )
plots[0].annotate('±5%',
             xy=(2.40, 1.05),    xytext=(2.50, 1.15),
             arrowprops=dict(arrowstyle="-", shrinkB=1.5),
             verticalalignment='center',
             fontsize=10
             )
plots[0].annotate('±2%',
             xy=(2.65, 1.02),    xytext=(2.75, 1.12),
             arrowprops=dict(arrowstyle="-", shrinkB=1.5),
             verticalalignment='center',
             fontsize=10
             )
plots[0].set_xlim([-0.5, 3])
plots[0].set_ylim([0, 1.5])
plots[0].set_xlabel('time(s)')
plots[0].set_title('Second-order system step response for '
          + '$\\omega_n = 2\\pi$ rad/s, '
          + '\nwith different damping factors',
          fontsize = 10
          )
plots[0].legend()
plots[0].grid()

##################################################
plots[1].plot([-0.5, 4], [0.95, 0.95], '-.', color='skyblue')
plots[1].plot([-0.5, 4], [1.05, 1.05], '-.', color='skyblue')
plots[1].plot([-0.5, 4], [0.98, 0.98], '-.', color='silver')
plots[1].plot([-0.5, 4], [1.02, 1.02], '-.', color='silver')

for k, wn in enumerate(omega_range):
    H = lti([wn*wn], [1, 2*zeta*wn, wn**2])
    _, y = H.step(T=t)
    plots[1].plot(t, y, label=F"$\\omega_n = {wn:.1f}$ rad/s")
    yl = y[40]
    tl = t[40]
    plots[1].annotate(F'$\\omega_n={wn}$ rad/s',
                 xy=(tl, yl),
                 xytext=(-0.45, yl),
                 arrowprops=dict(arrowstyle="-", shrinkB=1.5),
                 verticalalignment='center',
                 fontsize=10
                 )
plots[1].annotate('±5%',
             xy=(2.40, 1.05),    xytext=(2.50, 1.15),
             arrowprops=dict(arrowstyle="-", shrinkB=1.5),
             verticalalignment='center',
             fontsize=10
             )
plots[1].annotate('±2%',
             xy=(2.65, 1.02),    xytext=(2.75, 1.12),
             arrowprops=dict(arrowstyle="-", shrinkB=1.5),
             verticalalignment='center',
             fontsize=10
             )
plots[1].set_xlim([-0.5, 3])
plots[1].set_ylim([0, 1.5])
plots[1].set_xlabel('time')
plots[1].set_title('Second-order system step response for '
          + F'$\\zeta = {zeta:.3f}$ '
          + '\nwith different natural frequencies',
          fontsize = 10
          )
plots[1].legend()
plots[1].grid()
plt.show()
