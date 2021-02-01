import cv2
import numpy as np
import math
import serial
from socket import *
import time
import threading
import sys

#se indica la direccion y puerto del servidor que recibe todo los datos 
#el puerto por defecto es el 3001
socketInfo = ["localhost", 3001]

#funcion que estima el numero de dedos abiertos y cerrados en la mano a traves de la camara
#para imitar la pose en la la protesis robotica
def handPoseEstimation(socketInfo):
    
    #se inicia la conexion con el servidor protocolo udp
    socketCliente = socket(AF_INET, SOCK_DGRAM)
    accionmano=""
    lastestmano=0
    #inicia la captura de video
    cap = cv2.VideoCapture(0)
    #bucle que captura y procesa cada frame entregado por la camara
    while(cap.isOpened()):
        #se lee el frame y se guarda en 2 variables
        ret, img = cap.read()
        #se cres un recuadro donde se buscara obtener los datos de la pose de mano
        cv2.rectangle(img,(350,350),(90,90),(0,255,0),0)
        crop_img = img[90:350, 90:350]
        #se aplican filtros de color y difuminado
        grey = cv2.cvtColor(crop_img, cv2.COLOR_BGR2GRAY)
        value = (35, 35)
        blurred = cv2.GaussianBlur(grey, value, 0)
        #se crea una vista de contraste threshold
        _, thresh1 = cv2.threshold(blurred, 127, 255,cv2.THRESH_BINARY_INV+cv2.THRESH_OTSU)
        cv2.imshow('Thresholded', thresh1)
        #se buscan los contornos
        contours, hierarchy = cv2.findContours(thresh1.copy(),cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)
        max_area = -1
        #se guarda cada contorno
        for i in range(len(contours)):
            cnt=contours[i]
            area = cv2.contourArea(cnt)
            if(area>max_area):
                max_area=area
                ci=i
        
        cnt=contours[ci]
        #se dibuja un rectango con el area que contiene los contornos
        x,y,w,h = cv2.boundingRect(cnt)
        cv2.rectangle(crop_img,(x,y),(x+w,y+h),(0,0,255),0)
        #se crea una malla alrededor uniendo los puntos de los contronos encontrados
        hull = cv2.convexHull(cnt)
        drawing = np.zeros(crop_img.shape,np.uint8)
        cv2.drawContours(drawing,[cnt],0,(0,255,0),0)
        cv2.drawContours(drawing,[hull],0,(0,0,255),0)
        hull = cv2.convexHull(cnt,returnPoints = False)
        defects = cv2.convexityDefects(cnt,hull)
        count_defects = 0
        cv2.drawContours(thresh1, contours, -1, (0,255,0), 3)
        #se trasa lineas entre los puntos
        for i in range(defects.shape[0]):
            s,e,f,d = defects[i,0]
            start = tuple(cnt[s][0])
            end = tuple(cnt[e][0])
            far = tuple(cnt[f][0])
            a = math.sqrt((end[0] - start[0])**2 + (end[1] - start[1])**2)
            b = math.sqrt((far[0] - start[0])**2 + (far[1] - start[1])**2)
            c = math.sqrt((end[0] - far[0])**2 + (end[1] - far[1])**2)
            angle = math.acos((b**2 + c**2 - a**2)/(2*b*c)) * 57
           #si los angulos que generan las lineas son menores iguales a 90 se cuenta como un dedo encontrado 
            if angle <= 90:
                count_defects += 1
                cv2.circle(crop_img,far,1,[0,0,255],-1)
            
            cv2.line(crop_img,start,end,[0,255,0],2)
        
        #se define un accion deacuerdo al numero de dedos encontrados
        if count_defects == 1:
            accionmano="@"
        elif count_defects == 2:
            accionmano="#"
        elif count_defects == 3:
            accionmano="$"
        elif count_defects == 4:
            accionmano="%"
        else:
            accionmano="!"

        #se envia el numero de dedos abiertos para replicar en la protesis se envia un nuevo valor solo si el numero de dedos cambia 
        if lastestmano!=accionmano:
            socketCliente.sendto(accionmano.encode(), (socketInfo[0], socketInfo[1]))
            lastestmano=accionmano
            #print(accionmano)
            time.sleep(0.2)

        #se muentras las imagenes procesadas
        cv2.imshow('Gesture', img)
        all_img = np.hstack((drawing, crop_img))
        cv2.imshow('Contours', all_img)
        #se detiene la aplicacion al presinar la letra q o esc
        k = cv2.waitKey(10)
        if k == 27:
            cv2.destroyAllWindows()
            cap.release()
            socketCliente.close()
            break

#funcion que recibe los datos del acelerometro en la muneca para el control de la muneca de la protesis
def wristControl(socketInfo):
    #se inicia la conexion con el servidor protocolo udp
    socketCliente = socket(AF_INET, SOCK_DGRAM)
    #se configura la conexion serial que recibira los datos del acelerometro enviados por el microcontrolador
    #ser = serial.Serial('COM13', 38400) # Windows
    ser = serial.Serial('/dev/ttyUSB0', 38400) # Linux Ubuntu
    #bucle de control
    while True:
        #lee el comando recibido por puerto serie hasta el caracter finalizador "|" y envia los datos por socket al servidor de control
        try:
            comando = ser.read_until(b'|')
            socketCliente.sendto(comando, (socketInfo[0], socketInfo[1]))
        # se detiene la aplicacion con una interrupcion del teclado ctrl+c
        except KeyboardInterrupt:
            socketCliente.close()
            ser.close()
            print ("Interrupted")
            sys.exit(0)
#se generan dos hilos para correr las dos funciones en paralelo para no aftera la velocidad de la lectura y procesamiento de datos
t1 = threading.Thread(target = handPoseEstimation,args=(socketInfo,))
t2 = threading.Thread(target = wristControl,args=(socketInfo,))
#se inician las funciones
t1.start()
t2.start()