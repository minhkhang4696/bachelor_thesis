from drawnow import *
import socketserver

accel = []
plt.ion() #tell the matplotlib that we want to draw live data

def makeFig():
    plt.plot(accel)


class myTCPServer(socketserver.StreamRequestHandler):
    def handle(self):
            self.data = self.rfile.readline().strip()
            count = float(self.data)
            accel.append(count)
            drawnow(makeFig)
#create TCP server
if __name__ == "__main__":
    serv = socketserver.TCPServer(("localhost",5005),myTCPServer)
    serv.serve_forever()



