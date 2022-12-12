#!/usr/bin/env python3
# -*- coding:utf-8 -*-
###
# Project: <<project>>
# File:    second--order-ramp-response.py
# File Created: Thursday, 8th December 2022 10:25:57 pm
# Author: Peter Harrison 
# -----
# Last Modified: Thursday, 8th December 2022 10:45:00 pm
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
from scipy.signal import lti, lsim
t = np.arange(0, 3, 0.01)
n = len(t)
zeta_range = [0.3,  0.7, 1.0,  1.5]
omega_range = [2, 3, 5, 10]
ramp = np.array([50.0*x for x in t])
wn = 2*np.pi
zeta = 0.707

fig,plots = plt.subplots(2,sharex = True)
fig.suptitle('Second Order Ramp Responses',fontsize = 16)
fig.set_size_inches(8,10)

##########################################################
# ramp response - different bandwidths
plots[0].plot(t, ramp, 'c--', label='ramp')

for zeta in zeta_range:
    label = f'$\\zeta_n$ = {zeta:.3f}'
    H = lti([wn*wn], [1, 2*zeta*wn, wn**2])
    _, y, x = lsim(H, U=ramp, T=t)
    plots[0].plot(t, y, label=label)
plots[0].set_xlabel('time (s)')
plots[0].set_ylabel('ramp response')
plots[0].set_title('$2^{nd}$ order system ramp response for '
          + '$\\omega_n = 2\\pi$ rad/s, '
          + '\nwith different damping factors',
          fontsize = 10
          )
plots[0].legend(loc='lower right', fontsize=10)
plots[0].grid()

##################################################
##########################################################
# ramp response - different bandwidth
plots[1].plot(t, ramp, 'c--', label='ramp')
zeta = 0.707
for wn in omega_range:
    label = f'$\\omega_n$ = {wn} rad/s'
    H = lti([wn*wn], [1, 2*zeta*wn, wn**2])
    _, y, x = lsim(H, U=ramp, T=t)
    plots[1].plot(t, y, label=label)
plots[1].set_xlabel('time (s)')
plots[1].set_ylabel('ramp response')
plots[1].set_title('$2^{nd}$ order system ramp response for '
          + F'$\\zeta = {zeta}$, '
          + '\nwith different natural frequencies',
          fontsize = 10
          )
plots[1].legend(loc='lower right', fontsize=10)
plots[1].grid()
plt.show()
