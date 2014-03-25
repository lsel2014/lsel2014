#!/usr/bin/env python
#===============================================================================
# 
#  @author: J. Martin
#  20140320
# 
# 
#  Small program to test the Tx module with an UART-USB adapter.
#===============================================================================
ID = 'C'

import serial

ser = serial.Serial(
    port='/dev/ttyUSB0',\
    baudrate=2400,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS,\
        timeout=0)
        
print("connected to: " + ser.portstr)
a = 0
b = 0
while True:
    for c in ser.read():
        b+=1
        if c == ID:
            a+=1
            print("Evento " + str(a) + ": Tren Pasado")
        if b%10 == 0:
        	print ("Stats: " + str(a) + " good, " + str(b) + " total")
            

ser.close()
