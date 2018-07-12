from drawnow import *
import socketserver
import csv

accel = []
gyro = []
accel_char = []
gyro_char = []
plt.ion()  # tell the matplotlib that we want to draw live data
i = 0

def makeFig():
    plt.subplot(2, 1, 1)
    plt.plot(accel, 'r', label='Normalized Acceleration')
    plt.legend(loc='upper left')
    plt.ylim(0, 5)
    plt.subplot(2, 1, 2)
    plt.plot(gyro, 'b', label='Normalized Angular Velocity')
    plt.legend(loc='upper left')
    plt.savefig('testplot.png')
    with open("Hieu_5T_gyro.dat", "w",newline='') as myfilegyro:
        wr = csv.writer(myfilegyro)
        wr.writerow(gyro)
    with open("Hieu_5T_accel.dat", "w",newline='') as myfileaccel:
        wr1 = csv.writer(myfileaccel)
        wr1.writerow(accel)

class myTCPServer(socketserver.StreamRequestHandler):
    def handle(self):
        # read the data
        data = self.rfile.readline()
        # print (data)
        # split data in to separate components
        dataArray = data.decode().split(',')
        dataArray = dataArray[:-1]
        # append to float list
        for j in dataArray:
            index = dataArray.index(j)
            if index % 2 == 0:
                accel.append(float(j))
            else:
               gyro.append(float(j))
        drawnow(makeFig)
        print('\n')
server = socketserver.TCPServer(("", 8000), myTCPServer)
server.serve_forever()
