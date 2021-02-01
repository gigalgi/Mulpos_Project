#importamos las dependecias necesarias
import socketio
import base64
import cv2
import numpy as np

#configuramos los parametros de la camara para inicar la caputa de video
cap = cv2.VideoCapture(0)
cap.set(3,640)
cap.set(4,420)

#inicimos l conexxion con el servidor en nodejs como cliente
sio = socketio.Client()
sio.connect('http://localhost:3000')

#bucle para la adquisicion de imagen
while(cap.isOpened()):
    #leemos la imagen capturada por la camara
    ret, img = cap.read()
    
    #codificamos la imagen a formato jpeg y le vajamos la calidad al 50% para hacerla mas liviana
    image=cv2.imencode(".jpeg", img,[cv2.IMWRITE_JPEG_QUALITY, 50])[1]
    
    #codificamos la imagen en base 64
    img64=base64.b64encode(image)
    
    #agregamos la cabecera para que el navegador pueda decodificarla y enviamos como un string
    imgenco64 = "data:image/jpeg;base64," + img64.decode("utf-8") 
    
    #enviamos la imagen 
    sio.emit('stream', imgenco64)
    
    #podermos agregar un retraso en segundos en los envios si queremos recibir las imagenes cada x timpo
    #sio.sleep(x)
    
    #pre visualizacion de lo que estamos enviando
    #cv2.imshow('emitiendo',img)
    
    #detenemos todo presionando la tecla Esc 
    k = cv2.waitKey(10)
    if k == 27:
        sio.disconnect()
        cv2.destroyAllWindows()
        cap.release()
        break   

