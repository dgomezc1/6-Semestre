# Laboratorio 1 - Tópicos especiales en telemática
__David Gomez Correa__

__Código estudiante: 202017511010__

__Eafit__
                
----


- [Laboratorio 1](#laboratorio-1---tópicos-especiales-en-telemática)
  - [Nota importante](#nota-importante)
  - [Compilación y ejecucion](#compilación-y-ejecución)
  - [Diseño de solucion](#diseño-de-solucion)
  - [Detalles técnicos](#detalles-técnicos)


#### Nota importante: ####
El código presentado toma como base el Proyecto #2 de telemática presentado en el semestre 2022-1, realizado por David Gomez Correa (mi persona) y Samuel Meneses Diaz (Compañero del curso), dicho código fue modificado para atender a los requerimientos de este laboratorio.

#### compilación y ejecución ####
El laboratorio fue realizado utilizando el lenguaje de programación de Python, para su ejecución, únicamente es necesario tener instalado Python por defecto, ya que este incluye las librerías necesarias para su ejecución. Para correrlo, únicamente es necesario ejecutar el archivo Server.py con el siguiente comando `python Server.py`, con el cual iniciara la ejecución del proyecto.

#### Diseño de solucion ####
Como propuesta de solucion, se plantea un servidor minimalista, que con base al protocolo HTTP v0.9 es capaz de procesar Resquest GET, y retornar el recurso solicitado (en caso de existir) utilizando un HTTP Response.

Se tiene un archivo Server.py como el controlador de flujo principal, dicho archivo se encarga de las funciones más relevantes a nivel de conexión, como lo son:
- **creación de Socket y configuración de socket:** Se encarga de crear el socket, con base a una dirección IP preestablecida en el archivo *constans.py*, y un puerto también definido dentro de este mismo archivo. Cabe aclarar que dicho socket se configura para funcionar sobre el protocolo TCP.
- **Apertura de socket:** Se inicializa el socket con la dupla puerto - IP, posterior a ello se define un pull de conexiones máximo de 5, y se coloca visible dicho puerto.
- **Servidor concurrente:** Para poder configurar nuestro servidor como concurrente, dentro de la función *server_execution*, se utiliza la librería threading, que nos permite crear hilos, logrando de esta manera ejecutar el control de flujo de aplicación en un nuevo hilo, permitiendo de esta manera crear un servidor concurrente.
- **Control de flujo:**  La función *handler_client_connection*, es la encargada del control de flujo de las peticiones entrantes al servidor. Dicha funcion se encarga de decodificar el Request para evaluar si es aceptado el método utilizado, y en caso de serlo, reedirecionarlo a la clase get.py

Igualmente se tiene la clase *get.py*, que es la encargada de procesar las solicitudes get entrantes, dicha función se encarga de:
- **Procesamiento y lectura de archivos:** Con base a la dirección entregada dentro del HTTP Request, el método *get_object* se encarga de reescribir el path para posteriormente intentar leer el archivo y abrirlo; seguido a esto, se realiza la codificación del encabezado de respuesta y se adiciona el archivo en formato binario.

#### Detalles técnicos ####
librerías utilizadas:
- **scokets:** Es utilizada para la creación de nuestro canal de comunicación
- **threading:** librería utilizada que permite la creación de hilos de procesamiento, que permite al servidor ser concurrente al crear un hilo por cada petición entrante

distribución de carpetas:
- **/:** Dentro del Directorio raíz se tienen los archivos Server.py (control de flujo principal) y constans.py (Contenedor de variables constantes utilizadas por el programa)
- **/ServerMet:** Dentro de este directorio se encuentra el archivo get.py (control de flujo de las peticiones get) y el directorio Recursos.
- **/ServerMet/Recursos/:** Dentro de este directorio se encuentran todos los archivos utilizados por nuestra página web
