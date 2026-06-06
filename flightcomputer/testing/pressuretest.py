import numpy as np
import matplotlib.pyplot as plt


data = [
    (1406, 190), (1311, 2.2), (1318, 12), (1330, 30), (1351, 54),
    (1381, 100), (1432, 220), (1455, 250), (1485, 300), (1497, 340),
    (1539, 400), (1543, 460), (1581, 540), (1628, 630), (1663, 740),
    (1720, 860), (1786, 1000)
]# Convert data to NumPy arrays
data_np = np.array(data)# Separate x and y values
x = (data_np[:, 0])
y = data_np[:, 1]# Plot
# make linear reggression from the data
m, b = np.polyfit(x, y, 1)
plt.plot(x, m*x + b, label='y = {0:.2f}x + {1:.2f}'.format(m, b))
plt.plot(x, y, 'o', label='Data Points')
plt.title('Plot of Data Points')
plt.xlabel('X')
plt.ylabel('Y')
plt.grid(True)
plt.show()
