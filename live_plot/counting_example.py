from drawnow import *
import socketserver

count = []
plt.ion() #tell the matplotlib that we want to draw live data
def makeFig():
    plt.plot(data, 'b', label = 'counting')
    plt.savefig('testplot.png')

class myTCPServer(socketserver.StreamRequestHandler):
    def handle(self):
            data = self.rfile.readline()
            count.append(data)
            drawnow(makeFig)
#create TCP server
serv = socketserver.TCPServer(("",8000),myTCPServer)
serv.serve_forever()



