# UKMARSBOT MotorLab

Motorlab is a modified subset of the UKMARSBOT mazerunner code. It uses just one of the motor channels to drive a load that has similar characteristics to the actual robot. It is just a large, heavy wheel. 

Used with the motorlab-dashboard application, you can test controller settings in the same target environment that they will evenually on in your robot.

Primarily, the motorlab setup is a framework for demonstration and experimentation.

This README is not intended to be a comprehensive guide. It is just a note about sme features that I think you should know about. If you are reading this and need more information, firs examine the source code and then feel free to ask me to update the documentation. 

Or, of course, do a pull request for errors, omissions or additions.

You can find a video illustrating the use of this code at:
https://youtu.be/qKoPRacXk9Q
## UKMARSBOT preparation

To get going you can unplug the RIGHT MOTOR from the main board and plug in your own motor+encoder+load in its place. Alternatively, re-mount the existing motor further outboard and attach a larger wheel - one of the 70mm types from Kitronics would do or you can make your own.

If you have a suitably configured Bluetooth link to the robot, you could just use it as-is and operation will just spin the robot around its stationary left wheel. Bluetooth control will need to run at 115200 baud and you will need to modify the connection method in the dashboard code.

## Target code

The ukmarsbot-motorlab code in this directory will build with either vscode+platformio or the Arduino IDE. The setup() and loop() functions are in `main.cpp`. The files in this directory are those that you are most likely to want to modify. In particular, the `config.h and `config-motorlab.h` files hold information about things like the encoder resolution and gearbox ratio.

Code in the `src` directory is more library-like. The Arduino IDE will not show these files so that the window is not too cluttered. Even so, they are used and can be edited if you want to change things like the controller code - for example to experiment with a full PID or phase-lead compensator.

## Command Line Use

Once connected to the robot through a serial link (at 115200 baud) you can issue commands directly and observe the results. Commands all end with a line-feed character and carriage returns are ignored so be sure to set up your terminal application correctly.

At the time of writing there are 18 commands implemented. type a single question mark followed by the enter key to see a list:

```
      *IDN?     Request robot ID
      $         Display all Setting
      !         Write settings to EEPROM
      @         Read settings from EEPROM
      #         Initialise settings to defaults
      KM        Set/Get Km
      TM        Set/Get Tm
      KP        Set/Get Kp
      KD        Set/Get Kd
      ZETA      Set/Get Damping Ratio, zeta
      TD        Set/Get settling time, Td
      BIASFF    Set/Get bias feed forward
      SPEEDFF   Set/Get speed feedforward
      BATT      Get battery Voltage
      MOVE      Execute move profile
      STEP      Execute single step
      ENC       Set/Get KM
      VOLTS     Execute open loop
```

Many commands can accept additional parameters. For example, to move the drive using only feedforward through a distance of 2000 units with a top speed of 3600, a final speed of 0 and an acceleration of 5000, you can type

`   move 2 2000 3600 0 5000`

The target code will convert everything to upper case and defaults to eching the input back to the terminal. The command line can be edited with the backspace key as it is being typed but there is no "escape" that deletes the entire line.

Command options can be omitted from the end and will take default values. For example, to move using full control through a distance of 1600 you would just enter

`   move 0 1600`

### Echo

For use with software, such as the dashboard application, you can issue the command `echo off`. This will prevent the target echoing characters to the terminal. The echo can be re-enabled with `echo on`.

### Variables

Several of the commands allow you to get or set a variable. To query the value, just enter the variable name. For eample, enter `zeta` to display the current value of the damping ratio.

If you want to set a variable to a particular value, you type a command like `td = 0.134`. The value given will be assigned to the named variable. There is no general error checking but some variables have range limits. Examine the code in `commands.cpp` to find out more.

In fact, have a good look at `commands.cpp` and `robot.h` which is where most of the action is.


### Reset
If you mess up, just reset the robot or issue the command `#` which resets all variables to their default, compiled-in values.






