import matplotlib.pyplot as plt
import numpy as np

# Data
time = [0, 2, 4, 6, 8, 10, 12, 14, 16, 18]
altitude = [0.0, 3.0, 6.0, 8.0, 9.0, 8.5, 7.0, 4.0, 1.5, 0.0]
velocity_x = [0.0, 0.0, 0.5, 1.0, 1.5, 1.2, 0.8, 0.4, 0.0, 0.0]
velocity_y = [0.0, 0.0, 0.2, 0.5, 0.8, 0.6, 0.4, 0.2, 0.0, 0.0]
velocity_z = [0.0, 1.5, 1.0, 0.5, 0.0, -0.5, -1.0, -1.5, -1.0, 0.0]

# Create the plot
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 10))

# Plot altitude
ax1.plot(time, altitude, 'b-', label='Altitude')
ax1.set_xlabel('Time (s)')
ax1.set_ylabel('Altitude (m)')
ax1.set_title('Drone Altitude over Time')
ax1.legend()

# Plot velocities
ax2.plot(time, velocity_x, 'r-', label='Velocity X')
ax2.plot(time, velocity_y, 'g-', label='Velocity Y')
ax2.plot(time, velocity_z, 'b-', label='Velocity Z')
ax2.set_xlabel('Time (s)')
ax2.set_ylabel('Velocity (m/s)')
ax2.set_title('Drone Velocity Components over Time')
ax2.legend()

plt.tight_layout()
plt.show()
