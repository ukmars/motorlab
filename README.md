# UKMARS MotorLab

For the December meeting of the UK Micromouse and Robotics Society (UKMARS) there was a presentation form Peter Harrison about designing a PD controller for robot motor positioning.

This presentation focussed on a way to avoid tedious and error-prone tuning methods associated with the setting of the two parameters $K_p$ and $K_d$ needed for such controllers.

In essence, the approach is to use basic measurements of the system properties, along with some approximations for the standard-form second-order model of the drive system and calculate some starting values for these key parameters.

The basis for the calculations is that descriptions of the settling time and damping ratio for a system are more intuitive for the newcomer than trying to predict the effect of changing $K_p$ and $K_d$ directly.

The constants are calculated using relatively simple formulae expressed in terms of the damping ratio, $\zeta$ (zeta), and settling time, $T_D$. In many cases acceptable performance can be achieved in a single iteration. 

If feedforward is also part of the control scheme, it is likely that a very good performance can result with out repeated trials.

The principle is described and demonstrated in this recording of the presentation:

https://youtu.be/qKoPRacXk9Q

## Repository Structure

The repository as a whole is a platformio project that can be opened directly in VSCode with the PlatformIO extension. 

### `ukmarsbot-motorlab`

This directory holds the code that runs on the target system. It does the motor control and has a command-line interface that you can use directlly with a terminal on your PC. If you are using the Arduino IDE, open this directory in the IDE to make changes and downoad to the target.

Look at the README file for more information about the target application.

### `python`

In the `python` directory you will find a number of Python scripts. Many of these will simulate various reponses for first and second order systems, generating pretty charts for your continuing amusement.

Most important is the script `motorlab-dashboard.py`. This runs the dasboard application on your PC and lets you interact with the target to get or set variables and run commands the resuts of which are presented as charts.

Again, check the README file in that directory to find out more.

### `documents`

In here you should find some supporting files. Notably, there will be a copy of the presentation slides, in PDF format, and a paper describing how the calculations are derived.


