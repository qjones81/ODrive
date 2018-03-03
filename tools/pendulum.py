#!/usr/bin/env python3
"""
Example usage of the ODrive python library to monitor and control ODrive devices
"""

from __future__ import print_function

import odrive.core
import time
import math

# Find a connected ODrive (this will block until you connect one)
my_drive = odrive.core.find_any(consider_usb=True, consider_serial=False, printer=print)

print("Penulum Angle: " + str(my_drive.pendulum_angle))
print("Cart Position (m): " + str(my_drive.cart_position))

while True:
    print("Penulum Angle: " + str(my_drive.pendulum_angle))
    print("Cart Position (m): " + str(my_drive.cart_position))
    time.sleep(0.01)
