from builtins import input
import socket
import sys  

PORT = 4210

host = "192.168.1.102" 
family_addr = socket.AF_INET


try:
	sock = socket.socket(family_addr, socket.SOCK_DGRAM) 
except socket.error as msg:
	print("Failed to create socket")
	sys.exit()
 
while True:
	msg = input("Enter message to send : ")
	try:
		sock.sendto(msg.encode(), (host, PORT))
		reply, addr = sock.recvfrom(128)
		if not reply: break
		print("Reply[" + addr[0] + ":" + str(addr[1]) + "] - " + str(reply))
	except socket.error as msg:
		print("Error Code : " + str(msg[0]) + " Message: " + msg[1])
		sys.exit()

