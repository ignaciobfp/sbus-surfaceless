# sbus-surfaceless
A project for testing an VTOL plane with no control surfaces, only the two tilting motors

This is a very simple sketch for a personal project, which uses a flight controller with sbus output and a VTOL plane with two tilting motors and no control surfaces.

Since SBUS signal uses a considerably high baud rate (100.000 bps), reading this signal usually requires a dedicated hardware serial port on low-power microcontrollers. Arduino nano only has one hardware serial port, which makes it suboptimal for this project (no debugging is possible since that is the port used for USB communication), so a Teensy LC was used instead. Make sure to use the correct servo library for Teensy, since they have the same name the Arduino IDE might use the wrong one.

This Teensy microcontroller is connected to the sbus output of a Matek F765 Wing running ardupilot, and it does the following:

(1) If the current flight mode is a copter mode, it just passes the flight controller servo movements as-is
(2) If the current flight mode is a plane mode, read the servo position from flight controller, and add the corresponding movements from two pre-configured channel outputs from the flight controller (one for ailerons, one for elevator) to the tilt servos, so they change direction of the thrust and a similar effect of a control surface moving can be achieved.

The value of the flight mode channel is NOT present on the SBUS output on ardupilot, so it must be manually mixed in the transmitter.

This behaviour should be possible to implement properly in the flight controler software, but since I lack knowledge about the huge codebase and this implementation is for a prototype, it is an easier and effective solution for the time being.

This project was vaguely inspired by the control method used by Eflite in its Convergence VTOL rc model, which has control surfaces, but also moves the tilt motors to help a bit on rolls.

Recommended connection diagram, using a Y servo cable, splitting the signals and powering the servos from the FC 5V line (WARNING: Make sure your tilt servos maximum power consuption is not greater than the power provided by the flight controller, since SBUS output is not meant to power servos)

<a href="https://i.imgur.com/jo9Wan7.jpg"><img src="https://i.imgur.com/jo9Wan7l.jpg"></a>
