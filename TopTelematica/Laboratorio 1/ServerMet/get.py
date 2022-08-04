# ********************************************************************************************
    # Laboratorio #1
    # Course: ST0263 - Topicos Especiales en Telemática
    # MultiThread TCP-SocketServer
    #David Gomez Correa
# ********************************************************************************************


import constants
import re
from pathlib import Path
BASE_DIR = Path(__file__).parent.absolute()         #Tomamos el directorio base para obtener los recursos


def get_tipo(archivo):                              #Seleccion de tipo de archivo para encabezado
    if archivo.endswith('.jpg') or archivo.endswith('.jpeg'):
        tipo = "image/jpg"
    elif archivo.endswith('.png'):
        tipo = "image/png"
    elif archivo.endswith('.css'):
        tipo = "text/css"
    elif archivo.endswith('.pdf'):
        tipo = "application/pdf"
    elif archivo.endswith('.mp4'):
        tipo = "video/mp4"
    else:
        tipo = "text/html"
    return tipo

def get_object(address):                                #Metodo para retornar un recurso
    direction = address.split('?')[0]                   #Tomamos todo menos lo que hay despues del ?
    if direction == '/' or direction == '/home':        #En caso de que sea directorio base o /home
        archivo = str(BASE_DIR /'index.html')           #Archivo va a ser directorio base + index.html
        archivo = re.sub("[\\\]", "/", archivo)         #Se cambia los \ por /
    else:
        direction = direction[1:]                       #En caso de que no sea el index.html
        archivo = str(BASE_DIR / direction)             #Directorio base + la direccion que nos mandan
        archivo = re.sub("[\\\]", "/", archivo)        #Se cambia el \ por el /
    try:
        file = open(archivo, 'rb')                                          #Tratamos de leer el archivo
        response = file.read()
        file.close()
        #print(archivo)
        tipo_archivo = get_tipo(archivo)                 #Buscamos que de que tipo es el archivo
        header = constants.OK200+'Content-Type: '+str(tipo_archivo)+'\r\n\r\n'  #Preparamos el encabezado
    except Exception as e:
        print("Ocurrio un error")
        header = constants.Error404
        response = "".encode(constants.ENCONDING_FORMAT)                    #Encabezado para cuando no se encuentra el archivo
    print("\nResponse:",header)
    final_response = header.encode(constants.ENCONDING_FORMAT)              #Se codifica el encabezado y la respuesta
    final_response += response 
    return final_response                                                   #Se retorna la respuesta

get_object("/Recursos/imagenes/equipo.jpg")
