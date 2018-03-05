#!/usr/bin/env python3
"""
Example usage of the ODrive python library to monitor and control ODrive devices
"""

import odrive.core
import time
import math

# Find a connected ODrive (this will block until you connect one)
my_drive = odrive.core.find_any(consider_usb=True, consider_serial=False, printer=print)

my_drive.motor0.pos_gain = 50.0

my_drive.motor0.vel_gain = 5.0/10000.0
my_drive.motor0.vel_limit = 20000.0
my_drive.motor0.current_control.current_lim = 30.0

print("Setting")
my_drive.motor0.set_current_setpoint(2.0)
time.sleep(10.3)
print("Off")
my_drive.motor0.set_current_setpoint(0.0)
print("Done")