import json
import numpy as np
import matplotlib.pyplot as plt

# Load the sensor data from sensor_data.json
with open('kalman_filter_data.json', 'r') as file:
    sensor_data = json.load(file)

# Extract state vector data for position (assuming direct position data or interpreting velocity as displacement for visualization)
# Note: Since the setup was stationary, this example uses velocity as a proxy for displacement for illustrative purposes.
# In a dynamic scenario, you would integrate acceleration to get velocity and then position, or directly use position data if available.
position_x = np.array([entry['StateVector']['velocity_x'] for entry in sensor_data])
position_y = np.array([entry['StateVector']['velocity_y'] for entry in sensor_data])
position_z = np.array([entry['StateVector']['altitude'] for entry in sensor_data])

# Plotting the 3D trajectory
fig = plt.figure(figsize=(10, 7))
ax = fig.add_subplot(111, projection='3d')
ax.plot(position_x, position_y, position_z, label='Trajectory')
ax.set_xlabel('X Position')
ax.set_ylabel('Y Position')
ax.set_zlabel('Z Position')
plt.title('3D Trajectory Visualization')
plt.legend()
plt.show()



