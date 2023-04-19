# Control de posicion Robot Scara

Aqui encontraran 2 archivos:<br>
- Control_robot:<br>
    Se tienen 2 codigos para ESP32 por lo que se utilizaron ESP32 independientes para cada motor<br><br>
    - Control_robot_motor_1:<br>
        Aqui se encontrara el codigo diseñado en Arduino (C++) utilizando una ESP32 aplicando un Control PI y recibiendo variables de matlab el cual solo enviara la             trayectoria que se quiere realizar. Este es para el Motor 1<br><br>
    - Control_robot_motor_2:<br>
        Aqui se encontrara el codigo diseñado en Arduino (C++) utilizando una ESP32 aplicando un Control PI y recibiendo variables de matlab el cual solo enviara la             trayectoria que se quiere realizar. Este es para el Motor 2<br><br>
    - Lectura_Vel_Ang_Corr:<br>
        En este codigo, se aplica el control tanto para corriente como posicion en cascada para poder llevar el robot del punto A al punto B<br>
<br>

- control_PI<br>
    - ESP32_ina226:<br>
        El codigo que se encuentra aqui solo sirve para revisar que el sensor INA226 detecta la corriente de manera correcta aplicando un Control PI en discreto.<br>
    - Graficas de Velocidad de motores:<br>
        Aqui se encuentran datos y graficas en excel cargadas desde un codigo en python que permite ver la respuesta del sistema a una entrada.<br>
    - Prueba_GUI.py<br>
        GUI diseñada en python para enviar un tipo de control y luego ver la respuesta en 2 grafcias.<br>
