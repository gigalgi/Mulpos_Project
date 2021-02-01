from socket import *
import time
import sys
import serial

#seleccion de puerto
puerto = 3001
#configuarcion de servicor tipo udp
socketServidor = socket(AF_INET, SOCK_DGRAM)
#puerto en el cual se escucha
socketServidor.bind(('', puerto))
print('El servidor esta listo para recibir')

#configuracion de parametros comunicacion serial
#ser = serial.Serial('COM13', 38400) # Windows
ser = serial.Serial('/dev/ttyACM0', 38400) # Linux Ubuntu

#bucle de control
while True:
    
    #se ejecuta el try si hay llega informacion
    try:
        #verificamos si llega algo al socket
        accion, dirCliente = socketServidor.recvfrom(2048)
        #los datos resividos se decodifican de bytes a string
        accion = accion.decode('utf-8')
        #se codifica de string a bytestring y se envia por puerto serial
        if accion.encode()[:2] == b'\r\n':
            ser.write(accion.encode()[2:])
        else:
            ser.write(accion.encode())

        #print(accion)

    #se cierran las conexiones si hay una imterrupcion por teclado  
    except KeyboardInterrupt:
        ser.close()
        socketServidor.close()
        print ("Interrupted")
        sys.exit(0)
        
    
