"""
    This script serves as a visualizer for the COMPASS state estimation algorithm.
    It reads the state estimation data from the serial port and visualizes the orientation and acceleration of the sensor.
    TODO: read from sd card and visualize the trajectory and altitude, might be a good idea to add more dimensions to the visualization aka more internal windows
    - Brage
"""

import sys
import numpy as np
import quaternion as quat
import matplotlib.pyplot as plt
from PyQt5.QtCore import QTimer
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget, QPushButton
import time
import serial
import struct

DEVICE = '/dev/ttyUSB0'

class SensorWidget(QWidget):
    def __init__(self):
        super().__init__()

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_sensor_data_wrapper)
        self.timer.start(10)
        self.start_time = time.time()
        self.add_cube()
        self.initUI()



    def initUI(self):
        self.setWindowTitle('Sensor Visualization')
        self.fig = plt.figure()
        self.ax = self.fig.add_subplot(111, projection='3d')
        self.ax.set_xlabel('X', color='red')
        self.ax.set_ylabel('Y', color='green')
        self.ax.set_zlabel('Z', color='blue')
        self.ax.set_xlim([-3, 3])
        self.ax.set_ylim([-3, 3])
        self.ax.set_zlim([-3, 3])
        self.layout = QVBoxLayout()
        self.layout.addWidget(self.fig.canvas)
        self.setLayout(self.layout)
        self.quaternion = quat.quaternion(1, 0, 0, 0)
        self.show()




    def add_cube(self):
        vertices = np.array([
            [ 1.0,  1.0, 0.1], [ 1.0,  1.0, -0.1], 
            [ 1.0, -1.0, 0.1], [ 1.0, -1.0, -0.1],
            [-1.0, -1.0, 0.1], [-1.0, -1.0, -0.1], 
            [-1.0,  1.0, 0.1], [-1.0,  1.0, -0.1], 
            [ 0.0,  1.8, 0.1], [ 0.0,  1.8, -0.1]
        ])

        faces = np.array([  
            [0, 1, 3], [2, 0, 3], # right
            [2, 5, 3], [4, 5, 2], # back
            [4, 6, 7], [4, 7, 5], # left
            [0, 6, 2], [2, 6, 4], # top
            [1, 7, 3], [3, 7, 5], # bottom
            [0, 8, 1], [1, 8, 9], # left front
            [6, 7, 8], [7, 9, 8], # right front
            [6, 0, 8], # top front
            [7, 1, 9], # bottom front
        ])
        self.cube = (vertices, faces)



    def draw_cube(self):
        V, F = self.cube
        rotated_vertices = quat.rotate_vectors(self.quaternion, V)
        self.ax.plot_trisurf(rotated_vertices[:, 0], rotated_vertices[:, 1], rotated_vertices[:, 2], triangles=F,  color='green', alpha=0.4, shade=True)



    def update_sensor_data(self,sensor_data):
        """fake data with calculated quaternion comment this section when reading from serial port"""
        now = time.time()
        dt = now - self.start_time
        self.start_time = now
        q_dot = 0.5 * self.quaternion * quat.quaternion(0.0, 0.0, 0.0, 0.1)
        q_dot = q_dot * dt
        self.quaternion = self.quaternion + q_dot
        self.quaternion = self.quaternion.normalized()

        aceleration = np.array([[0, 0, 9.81]]) * 0.12       
        aceleration = quat.rotate_vectors(self.quaternion, aceleration)
        aceleration = quat.rotate_vectors(self.quaternion.conjugate(), aceleration)
        aceleration = aceleration.flatten()
        """end fake data section"""

        """real data from sensor comment this section when using fake data"""
        # self.quaternion = quat.quaternion(*sensor_data[0:4])
        # aceleration = np.array([sensor_data[4:7]]) * 0.12
        # aceleration = aceleration.flatten()
        """end real data section"""
        
        
        self.ax.clear()
        self.draw_cube()
        # draw aceleration vector
        self.ax.quiver(0, 0, 0, aceleration[0], aceleration[1], aceleration[2], color='black')
        # draw center of mass
        self.ax.scatter(0, 0, 0, color='black', s=40)

       
        self.ax.set_xlabel('X', fontsize=16, fontweight='bold')
        self.ax.set_ylabel('Y', fontsize=16, fontweight='bold')
        self.ax.set_zlabel('Z', fontsize=16, fontweight='bold')
        self.ax.set_xlim([-3, 3])
        self.ax.set_ylim([-3, 3])
        self.ax.set_zlim([-3, 3])
        self.fig.canvas.draw()
        # self.fig.text(0.5, 0.95, 'altitude: 120.0 m', ha='center', va='center', fontsize=16, fontweight='normal')



    def reset_orientation(self):
        self.quaternion = quat.quaternion(1, 0, 0, 0)




    def update_sensor_data_wrapper(self):
        """read data from serial port comment this section when using fake data"""
        # ser = serial.Serial(DEVICE, 115200)
        # a,b,c,d,e,f,g = 0,0,0,0,0,0,0
        # try:   
        #     line = ser.read(60)
        #     start = line.find(b'\n')
        #     end = line.find(b'\n', start+1)
        #     data = line[start+1:end]
        #     a,b,c,d,e,f,g = struct.unpack('<7f', data)
        #
        #
        # except:
        #     print('Error reading data')
        #     a,b,c,d = quat.as_float_array(self.quaternion)
        #     e,f,g = 0,0,0
        # finally:
        #     ser.close()
        #
        # serial_data = [a,b,c,d,e,f,g]
        # self.update_sensor_data(serial_data)
        """end read data from serial port"""

        """fake data comment this section when reading from serial port"""
        self.update_sensor_data([0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 9.81])
        """end fake data"""

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.sensor_widget = SensorWidget()
        self.setCentralWidget(self.sensor_widget)
        self.initUI()

    def initUI(self):
        self.setWindowTitle('Sensor Visualization')
        self.setGeometry(600, 250, 1400, 1100)

        self.btn_update = QPushButton('Reset', self)
        self.btn_update.setGeometry(10, 10, 150, 30)
        self.btn_update.clicked.connect(self.sensor_widget.reset_orientation)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    mainWindow = MainWindow()
    mainWindow.show()
    sys.exit(app.exec_())
