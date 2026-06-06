import json
import matplotlib.pyplot as plt

# Load JSON data
with open('sensor_data.json', 'r') as file:
    sensor_data = json.load(file)

# Initialize lists to hold the time (msAfterLaunch) and sensor values
time = []
accel_x = []
pressure = []

# Populate the lists with data
for entry in sensor_data:
    time.append(entry['msAfterLaunch'])
    accel_x.append(entry['accelerometerData']['x'])
    pressure.append(entry['pressureSensorData'])

# Create plots
fig, ax1 = plt.subplots()

# Plot Accelerometer X-axis data
color = 'tab:red'
ax1.set_xlabel('Time (msAfterLaunch)')
ax1.set_ylabel('Accel X', color=color)
ax1.plot(time, accel_x, color=color)
ax1.tick_params(axis='y', labelcolor=color)

# Instantiate a second y-axis to plot pressure
ax2 = ax1.twinx()  
color = 'tab:blue'
ax2.set_ylabel('Pressure', color=color)  
ax2.plot(time, pressure, color=color)
ax2.tick_params(axis='y', labelcolor=color)

# Show the graph
fig.tight_layout()  
plt.title('Sensor Data Over Time')
plt.show()

