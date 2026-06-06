
import json
import numpy as np
import matplotlib.pyplot as plt
from scipy.integrate import cumtrapz

# Get data from sensor_data.json
with open('sensor_data.json', 'r') as file:
    sensor_data = json.load(file)

# Assuming convert msAfterLaunch to seconds
time = np.array([entry['msAfterLaunch'] for entry in sensor_data]) / 1000.0
dt = np.diff(time)

# Extract and integrate acceleration data
accel_x = np.array([entry['accelerometerData']['x'] for entry in sensor_data])
accel_y = np.array([entry['accelerometerData']['y'] for entry in sensor_data])
accel_z = np.array([entry['accelerometerData']['z'] for entry in sensor_data])

# Since the data is filtered and corrected, integrate directly to get velocity and then position
velocity_x = cumtrapz(accel_x, time, initial=0)
position_x = cumtrapz(velocity_x, time, initial=0)

velocity_y = cumtrapz(accel_y, time, initial=0)
position_y = cumtrapz(velocity_y, time, initial=0)

velocity_z = cumtrapz(accel_z, time, initial=0)
position_z = cumtrapz(velocity_z, time, initial=0)

# Plotting the 3D trajectory
fig = plt.figure(figsize=(10, 7))
ax = fig.add_subplot(111, projection='3d')
ax.plot(position_x, position_y, position_z, label='Trajectory')
ax.set_xlabel('X Position')
ax.set_ylabel('Y Position')
ax.set_zlabel('Z Position')
plt.title('KON-TIKI Trajectory!!!')
plt.legend()
plt.show()
