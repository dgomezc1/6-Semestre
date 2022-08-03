# ********************************************************************************************
    # Laboratorio #1
    # Course: ST0263 - Topicos Especiales en Telemática
    # MultiThread TCP-SocketServer
    #David Gomez Correa
# ********************************************************************************************

# Librerias importadas, socket para comunicacion, threading para concurrencia, constants para variables staticas
# clase get

import socket
import threading
import constants

from ServerMet import get

# Declaracion del objecto socket...
server_socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)           #AF_INET define el tipo de direccion (ipv4), y modo TCP
server_address = constants.IP_SERVER                                       #Define la direccion del servidor
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)        #Configuracion del socket

def main():
    print("***********************************")
    print("Servidor corriendo...")
    print("Dir IP:",server_address )
    print("Puerto:", constants.PORT)
    server_execution()
    
# Funcion encargada del manejo de las peticiones...

def handler_client_connection(client_connection,client_address):
    print(f'Nueva conexion entrante desde: {client_address[0]}:{client_address[1]}')
    is_connected = True
    
    data_recevived = client_connection.recv(constants.RECV_BUFFER_SIZE)             #Le los datos obetnidos de la peticion
    if data_recevived == b'':   
        print(f'\nAhora, cliente {client_address[0]}:{client_address[1]} esta desconectado...')
        client_connection.close()
        return
    print (f'\nDatos recividos desde: {client_address[0]}:{client_address[1]}')     #Imprimimos de donde nos llega la conexion
    remote_string = data_recevived.split(b'\r\n\r\n')                               #Division de la peticion entrante por contenido y header
    header = str(remote_string[0].decode(constants.ENCONDING_FORMAT))               #Tomamos la posicion 1 que es el header y decodificamos                                                     
    print("Request: \n")
    print(header)                                                                   #Imprimimos el comando entrante
    header = header.split()                                                         #Dividimos el header por  ' '
    command = header[0]                                                             #El comando va posicion 0 del header
    if (command == constants.GET):                                                  #En caso de que el comando sea GET
        response = get.get_object(header[1])                                        #Enviamos el header[1] es la direccion del objecto que desea tener
        client_connection.sendall(response)
    else:                                                                                               #En caso se de llegar un metodo no disponible
        print("\nResponse: \n:",constants.Error400)
        response = constants.Error400.encode(constants.ENCONDING_FORMAT)                                #Cargamos una cabecera 400 para hacer referencia a una mala peticion
        client_connection.sendall(response.encode(constants.ENCONDING_FORMAT))                          #Cargamos mensaje de error y enviamos
    print(f'\nAhora, cliente {client_address[0]}:{client_address[1]} esta desconectado...')             #Desconectamos el cliente
    client_connection.close()                                                               

#Funcion que comienza el servidor..
def server_execution():
    tuple_connection = (server_address,constants.PORT)                      #Tupla con la direccion y el puerto
    server_socket.bind(tuple_connection)                                    #Colocamos el socket visible en privado (dupla direccion, puerto)
    print ('Socket vinculado a direccion y puerto...')
    server_socket.listen(5)                                                 #Indicamos a sockets no colocar mas de 5 solicitudes en cola
    print('Socket is listening...')
    while True:       
        try:                                                       #Mantenemos el servidor escuchando peticiones por el puerto designado
            client_connection, client_address = server_socket.accept()          #Leemos si hay una conexion entrante
            client_thread = threading.Thread(target=handler_client_connection, args=(client_connection,client_address)) #Usamos threading para abrir un hilo y hacer nuestro servidor concurrente
            client_thread.start()                                               #Comenzamos el hilo
        except:
            print("A ocurrido un error en el servidor")
            break
    print('Socket is closed...')
    server_socket.close()

if __name__ == "__main__":
    main()