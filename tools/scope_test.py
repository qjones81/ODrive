#!/usr/bin/env python3
"""
Example usage of the ODrive python library to monitor and control ODrive devices
"""

from __future__ import print_function

import odrive.core
import time
import math
import matplotlib.pyplot as plt
import numpy as np
import threading

# Find a connected ODrive (this will block until you connect one)
my_drive = odrive.core.find_any(consider_usb=True, consider_serial=False, printer=print)

plt.ion()
global vals
vals = []

def fetch_data():
    global vals
    while True:
        vals.append(my_odrive.pendulum_angle)
        if len(vals) > num_samples:
            vals = vals[-num_samples:]
        time.sleep(1/data_rate)

# Wait for scope to be triggered
while True:
    trigger_complete = my_drive.scope.trigger_complete
    print("Triggering: " + str( my_drive.scope.is_triggering))
    print("Trigger Complete: " + str(trigger_complete))
    print("Pendulum Angle: " + str(my_drive.pendulum_angle))
    if trigger_complete:
        break

    time.sleep(0.1)
# Read Channel data

print("OUT!")
channel_1 = 0
my_drive.scope.read_sample_buffer_size(channel_1)
buffer_size = my_drive.scope.sample_buffer_size
print("Buffer Size: " + str(buffer_size))

# Read back samples
index = 0
vals = []
while (index < buffer_size):
    my_drive.scope.read_sample(channel_1, index)
    sample_value = my_drive.scope.sample_value
    #print("Sample: " + str(sample_value))
    vals.append(sample_value)
    index = index + 1
# Live plot
plt.ion()


while True:
    plt.clf()
    # for series in vals:
    #     plt.plot(series)
    plt.plot(vals)
    plt.pause(1)

time.sleep(30)

# print("Penulum Angle: " + str(my_drive.pendulum_angle))
# print("Cart Position (m): " + str(my_drive.cart_position))

# while True:
#     print("Penulum Angle: " + str(my_drive.pendulum_angle))
#     print("Cart Position (m): " + str(my_drive.cart_position))
#     time.sleep(0.01)
