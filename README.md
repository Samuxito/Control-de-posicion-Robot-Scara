# Control de posicion Robot Scara
 
Aqui encontraran 2 archivos:
1.) Control_robot:
    Archivos a encontrar:
    Se tienen 2 codigos para ESP32 por lo que se utilizaron ESP32 independientes para cada motor
    a.) Control_robot_motor_1:
        Aqui se encontrara el codigo diseñado en Arduino (C++) utilizando una ESP32 aplicando un Control PI y recibiendo variables de matlab el cual solo enviara la             trayectoria que se quiere realizar. Este es para el Motor 1
    b.) Control_robot_motor_1:
        Aqui se encontrara el codigo diseñado en Arduino (C++) utilizando una ESP32 aplicando un Control PI y recibiendo variables de matlab el cual solo enviara la             trayectoria que se quiere realizar. Este es para el Motor 2
    
    c.) Lectura_Vel_Ang_Corr:
        En este codigo, se aplica el control tanto para corriente como posicion en cascada para poder llevar el robot del punto A al punto B

2.) control_PI
    a.) ESP32_ina226:
        El codigo que se encuentra aqui solo sirve para revisar que el sensor INA226 detecta la corriente de manera correcta aplicando un Control PI en discreto.
    b.) Graficas de Velocidad de motores:
        Aqui se encuentran datos y graficas en excel cargadas desde un codigo en python que permite ver la respuesta del sistema a una entrada.
    c.) Prueba_GUI.py
        GUI diseñada en python para enviar un tipo de control y luego ver la respuesta en 2 grafcias.
