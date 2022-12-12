#!/usr/bin/env python3
# -*- coding:utf-8 -*-
###
# Project: <<project>>
# File:    motorlab-simulation-profile-response.py
# File Created: Thursday, 8th December 2022 10:25:57 pm
# Author: Peter Harrison
# -----
# Last Modified: Friday, 9th December 2022 10:57:57 pm
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

import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import lti, lsim

##########################################################
# profile used in motorlab demo
# - accelerate at 14400 deg/s/s to 3600 deg/s
# - hold for 150ms
# - decelerate to stationary at 14400 deg/s/s
# - Final segment allows system to stabilise
# total distance travelled is 1440 deg.
# there is likely a better way to generate the profile
t0 = 0.0
t1 = 0.250
t2 = 0.400
t3 = 0.650
t4 = 0.850
step = 0.002001
MAX = 3600
ACCELERATION = MAX / (t1-t0)
p1 = [ACCELERATION * t for t in np.arange(t0, t1, step)]
p2 = [MAX for t in np.arange(t1, t2, step)]
ACCELERATION = MAX/(t3-t2)
p3 = [MAX - ACCELERATION * (t - t2) for t in np.arange(t2, t3, step)]
p4 = [0 for t in np.arange(t3, t4, step)]
profile = np.append(p1, p2)
profile = np.append(profile, p3)
profile = np.append(profile, p4)
t = np.arange(t0, t4, step)
##########################################################

# System gain and time constant from motorlab demonstration
KM = 2065
TM = 0.325

##########################################################
fig, plots = plt.subplots(2, sharex=True)
fig.set_size_inches(8, 10)
fig.suptitle('Motorlab Ideal Profile Responses\n(infinite motor drive assumed)',
             fontsize=16)

##########################################################
# vary Kp, fix Kd
plots[0].plot(t, profile, 'k--', label='profile')
kd = 0.007
kp_range = [2.0, 0.2, 0.02]
for kp in kp_range:
    num = [kd*KM/TM, kp*KM/TM]
    den = [1, (kd*KM+1)/TM, kp*KM/TM]
    label = f'$K_p$ = {kp:.3f}'
    H = lti(num, den)
    _, y, x = lsim(H, U=profile, T=t)
    plots[0].plot(t, y, label=label)

plots[0].set_ylim([-500, 4500])
plots[0].set_yticks(range(0, 4500, 500))
plots[0].set_ylabel('angular velocity (deg/s)')
plots[0].set_xticks(np.arange(0, 0.81, 0.1))
plots[0].set_xlabel('time (s)')
plots[0].set_title(F'$K_d = {kd:.3f}$, varying $K_p$', fontsize=10)
plots[0].legend(loc='upper right', fontsize=10)
plots[0].grid()

########################################
# fix Kp, vary Kd
plots[1].plot(t, profile, 'k--', label='profile')
kp = 0.200
kd_range = [0.002, 0.007, 0.012]
kd_range = [0.07, 0.007, 0.0007]
for kd in kd_range:
    num = [kd*KM/TM, kp*KM/TM]
    den = [1, (kd*KM+1)/TM, kp*KM/TM]
    label = f'$K_d$ = {kd:.3f}'
    H = lti(num, den)
    _, y, x = lsim(H, U=profile, T=t)
    plots[1].plot(t, y, label=label)

plots[1].set_ylim([-500, 4500])
plots[1].set_yticks(range(0, 4500, 500))
plots[1].set_ylabel('angular velocity (deg/s)')
plots[1].set_xticks(np.arange(0, 0.81, 0.1))
plots[1].set_xlabel('time (s)')
plots[1].set_title(F'$K_p = {kp:.3f}$, varying $K_d$', fontsize=10)
plots[1].legend(loc='upper right', fontsize=10)
plots[1].grid()

plt.show()
