# Python Scripts

## Dashboard

The `motorlab-dashboard.py` script is the entire host-side application that runs the demo.

To run it you will need Python 3. Probably, any recent version will work but I have only tested against 3.10.

Requirements:
- numpy
- pyqt5
- pyqtgraph
- pyserial

The dashboard expects to connect to, and communicate with, a standard UKMARSBOT with an Arduino Nano processor, running the `ukmarsbot-motorlab` code.

Once running, the dashboard will continually scan for a serial port and automatically connects, at 115200 baud, when it finds one. Configuration data is then uploaded from the target. and the application is ready to use.

Successful connection is indicated by a green message in the status bar.

If the target is disconnected, the dashboard will revert to scanning for a new connection.

**NOTE: if there is more than one valid USB serial bridge connected to the computer, the dashboard will connect to the first one it finds. Connect only one target to avoid errors.**

The target is driving a single motor on one of its normal motor drive channels. the motor has a large wheel attached to give a reasonable inertial load such that the rise time is about 0.3 seconds. characterising the gain and time constant of the system is one of the tasks that should be performed during setup.

If you have a radio link, the code could run on a free-standing robot performing turns or linear motion. you would need to modify the serial connection code for this to workIt is much easier, and safer to run the demo on a static bench model.

Any other suitable processor plus motor plus load target could be used with suitable modification to the code.


## Response Plots

As well as the main dashboard code, there are five other python scripts used only to generate plots of the main response characteristics. These are not needed for the dashboard. the scripts have different requirements for Python modules:

You will need:
- matplotlib
- scipy
- numpy

The script title should be fairly self-explanatory.

One of the scripts simulated the actual profile used by the motorlab target. It is there not so much to show 'good' values for the controller constants Kp and Kd but to act as a reminder that simulations can be horribly misleading. If you run the script `motorlab-simulation-profile-response.py' you might come away thinking that large values for Kp and Kd are the answer to your prayers. however, the simulation assumes two key features.

First, that the encoder feedback has infinite resolution. If that is so, then there is no noise in the derivative term and so the derivative component is well behaved.

Second, that the motor drive has an infinite available headroom. That is, whatever Voltage the controller asks for, it can deliver.

Neither of these things are true in the motorlab hardware, or pretty much any other real hardware. Consequently there is a practical limit on the magnitude of both constants if you are to avoid saturating the motor driver or intriducing large anounts of noise due to the use of low resolution encoders.



