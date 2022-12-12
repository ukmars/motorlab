#!/usr/bin/env python3
# -*- coding:utf-8 -*-
###
# Project: <<project>>
# File:    second--order-profile-response.py
# File Created: Thursday, 8th December 2022 10:25:57 pm
# Author: Peter Harrison 
# -----
# Last Modified: Sunday, 11th December 2022 4:00:56 pm
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


##########################################################
# profile response - different damping ratio
# I expect there is a better way to generate the profile
t0 = 0.0
t1 = 1.0
t2 = 7.0
t3 = 11.0
t4 = 15.0
step = 0.01
MAX = 10
SLOPE = MAX / (t1-t0)
p1 = [SLOPE * t for t in np.arange(t0, t1, step)]
p2 = [MAX for t in np.arange(t1, t2, step)]
SLOPE = MAX/(t3-t2)
p3 = [MAX - SLOPE * (t - t2) for t in np.arange(t2, t3, step)]
p4 = [0 for t in np.arange(t3, t4, step)]
profile = np.append(p1, p2)
profile = np.append(profile, p3)
profile = np.append(profile, p4)
t = np.arange(t0, t4, step)


fig,plots = plt.subplots(2,sharex = True)
fig.suptitle('Second Order Profile Responses',fontsize = 16)
fig.set_size_inches(8,10)

##########################################################
plots[0].plot(t, profile, 'c--', label='profile')

for zeta in zeta_range:
    label = f'$\\zeta$ = {zeta:.3f}'
    H = lti([wn*wn], [1, 2*zeta*wn, wn**2])
    _, y, x = lsim(H, U=profile, T=t)
    plots[0].plot(t, y, label=label)
plots[0].set_xlabel('time (s)')
plots[0].set_ylabel('ramp response')
plots[0].set_title('$2^{nd}$ order system profile response for '
          + '$\\omega_n = 2\\pi$ rad/s, '
          + '\nwith different damping ratios',
          fontsize = 10
          )
plots[0].legend(loc='upper right', fontsize=10)
plots[0].grid()

##################################################
##########################################################
plots[1].plot(t, profile, 'c--', label='profile')
zeta = 0.707
for wn in omega_range:
    label = f'$\\omega_n$ = {wn} rad/s'
    H = lti([wn*wn], [1, 2*zeta*wn, wn**2])
    _, y, x = lsim(H, U=profile, T=t)
    plots[1].plot(t, y, label=label)
plots[1].set_xlabel('time (s)')
plots[1].set_ylabel('ramp response')
plots[1].set_title('$2^{nd}$ order system profile response for '
          + F'$\\zeta = {zeta}$, '
          + '\nwith different natural frequencies',
          fontsize = 10
          )
plots[1].legend(loc='upper right', fontsize=10)
plots[1].grid()


plt.show()
