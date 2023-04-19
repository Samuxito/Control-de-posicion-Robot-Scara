from ctypes import sizeof
import time, collections
import tkinter as tk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.lines import lineStyles
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import serial
import threading

Detener_control = threading.Event()
refer=0; corriente=0; error=0; señal_contr=0;x=0;

ventana = tk.Tk()
ventana.geometry('1200x760')
ventana.wm_title('Tipos de controles de Torque')
ventana.minsize(width=1200, height=760)
ventana.config(bd=15)
opcion = tk.StringVar(ventana)
tipo_entrada = tk.StringVar(ventana)
ref = tk.DoubleVar(ventana)

def lectura(dec, name1):
    global refer, corriente, error, señal_contr, x
    Detener_control.clear()
    arduinoSerial.write(dec.encode('ascii'))
    time.sleep(3)
    while arduinoSerial:
        try:
            valores = arduinoSerial.readline().decode('ascii')
        except:
            pass
        dat_analisis = valores.split(',')
        refer = float(dat_analisis[0])
        corriente = round(float(dat_analisis[1]),2)
        error = round(float(dat_analisis[2]),2)
        señal_contr = round(float(dat_analisis[3]),1)
        x=x+0.01
        if Detener_control.is_set():
            break

def grafica_1(i, r, cor, x_lenght1, err):
    global refer, corriente, error, señal_contr, x
    r.append(refer)
    cor.append(corriente)
    err.append(error)
    x_lenght1.append(x)
    r = r[-50:]
    cor = cor[-50:]
    err = err[-50:]
    x_lenght1 = x_lenght1[-50:]
    ax1.clear()
    ax1.plot(x_lenght1, r,'r',x_lenght1, cor,'b',x_lenght1, err, 'g')
    ax1.set_title("Ref/Corriente/Error vs Tiempo",color='red',size=16,family="Arial")
    ax1.set_ylim([-0.5,0.5])
    ax1.set_xlabel("Tiempo", color="black")
    ax1.set_ylabel("Corriente/Error", color="black")
    ax1.set_facecolor('#D5DBDB')
    ax1.grid(color="black")

def grafica_2(i, contr, x_lenght2):
    global refer, corriente, error, señal_contr, x
    contr.append(señal_contr)
    x_lenght2.append(x)
    contr = contr[-50:]
    x_lenght2 = x_lenght2[-50:]
    ax2.clear()
    ax2.plot(x_lenght2, contr,'g')
    ax2.set_title("Señal Control vs Tiempo",color='red',size=16,family="Arial")
    ax2.set_ylim(-12,12)
    ax2.set_facecolor('#D5DBDB')
    ax2.set_xlabel("Tiempo", color="black")
    ax2.set_ylabel("Señal Control", color="black")
    ax2.grid(color="black")

def detener_datos():
    Detener_control.set()
    detener = str("{:.2f}".format(0.00))+"A"+"2"
    arduinoSerial.write(detener.encode('ascii'))
    ax1.reset_position()
    ax2.reset_position()

def lectura_datos():
    global refer, corriente, error, señal_contr, x
    envio = str("{:.2f}".format(ref.get()))+tipo_entrada.get()+opcion.get() 
    print(envio)
    Envio_recepcion = threading.Thread(target=lectura, args=(envio, 'Graficas'))
    Envio_recepcion.start()
    time.sleep(6)
    anim1 = animation.FuncAnimation(fig1, grafica_1, frames=100, fargs=(r, cor, x_lenght1, err), interval = 100)
    anim2 = animation.FuncAnimation(fig2, grafica_2, frames=100, fargs=(contr, x_lenght2), interval = 100)
    plt.show()

r=[];cor=[];err=[];contr=[];x_lenght1=[];x_lenght2=[];

fig1 = plt.figure(dpi=90,facecolor='#85C1E9', figsize=(5.5,4))
ax1 = fig1.add_subplot(111)
ax1.set_title("Ref/Corriente/Error vs Tiempo",color='red',size=16,family="Arial")
ax1.set_ylim([-0.5,0.5])
ax1.set_xlabel("Tiempo", color="black")
ax1.set_ylabel("Corriente/Error", color="black")
ax1.set_facecolor('#D5DBDB')
ax1.grid(color="black")

fig2 = plt.figure(dpi=90, figsize=(5.5,4),facecolor='#85C1E9')
ax2 = fig2.add_subplot(111)
ax2.set_title("Señal Control vs Tiempo",color='red',size=16,family="Arial")
ax2.set_ylim(-12,12)
ax2.set_facecolor('#D5DBDB')
ax2.set_xlabel("Tiempo", color="black")
ax2.set_ylabel("Señal Control", color="black")
ax2.grid(color="black")

arduinoSerial = serial.Serial('COM4', 115200)

gui = tk.Frame(ventana, bg='#85929A', width=1900, height=760)
#gui.grid(column=1,row=1)
gui.pack(fill='both')

canvas1 = FigureCanvasTkAgg(fig1, master = gui)
canvas1.get_tk_widget().grid(column=0,row=7,padx=2, pady=5)
canvas2 = FigureCanvasTkAgg(fig2, master = gui)
canvas2.get_tk_widget().grid(column=1,row=7,padx=2, pady=5)

tk.Label(gui, width=30, text="Tipo de Control", bg='#1E8449').grid(column=0, row=0,sticky="W")
tk.Label(gui, width=50, text=" ", bg='#1E8449').place(x=200,y=0)

tk.Button(gui, text='Realizar Control', width=15, bg='#DC7633', fg='white',command=lectura_datos).grid(sticky="W", column=0, row=6, padx=5, pady=5)
tk.Button(gui, text='Detener Control', width=15,bg='#FF0000', fg='white', command=detener_datos).grid(column=0, row=6, padx=5, pady=5)

tk.Radiobutton(gui, text="Continuo-Discreto  ", variable=opcion, value=1, bg='#85929A').grid(column=0, row=1, pady=5, sticky="W")
tk.Radiobutton(gui, text="Discreto-Discreto  ", variable=opcion, value=2, bg='#85929A').grid(column=0, row=2, pady=5, sticky="W")
tk.Radiobutton(gui, text="Servo sistema      ", variable=opcion, value=3, bg='#85929A').grid(column=0, row=3, pady=5, sticky="W")
tk.Radiobutton(gui, text="Compensador en Freq", variable=opcion, value=4, bg='#85929A').grid(column=0, row=4, pady=5, sticky="W")
tk.Radiobutton(gui, text="Tiempo Minimo      ", variable=opcion, value=5, bg='#85929A').grid(column=0, row=1, pady=5)
tk.Radiobutton(gui, text="Anulacion de Planta", variable=opcion, value=6, bg='#85929A').grid(column=0, row=2, pady=5)
tk.Radiobutton(gui, text="LGR - Compensador  ", variable=opcion, value=7, bg='#85929A').grid(column=0, row=3, pady=5)
tk.Radiobutton(gui, text="Oscilacion Muertas ", variable=opcion, value=8, bg='#85929A').grid(column=0, row=4, pady=5)

tk.Radiobutton(gui, text="Entrada PWM  ", variable=tipo_entrada, value="A", bg='#85929A').grid(column=0, row=1, pady=5, sticky="E")
tk.Radiobutton(gui, text="Entrada Rampa", variable=tipo_entrada, value="B", bg='#85929A').grid(column=0, row=2, pady=5, sticky="E")
tk.Radiobutton(gui, text="Entrada Sin  ", variable=tipo_entrada, value="C", bg='#85929A').grid(column=0, row=3, pady=5, sticky="E")

tk.Label(gui, text="Referencia", bg='#E6E6E6').grid(sticky="W", column=0, row=5, padx=5, pady=5)
referencia = tk.Entry(gui,bg='#E6E6E6',textvariable=ref)
referencia.place(x=100,y=165)

ventana.mainloop()