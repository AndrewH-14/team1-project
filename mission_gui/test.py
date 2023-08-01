import PySimpleGUI as sg
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np
import csv

global flag

# Read the barrier synthetic dataset
barrier_file = open('barrier.dat', 'r')
reader = csv.reader(barrier_file)
barrier_dat = []
for record in reader:
    barrier_dat.append(record)

# Read the barrier synthetic dataset
line_following_file = open('line_following.dat', 'r')
reader = csv.reader(line_following_file)
line_following_dat = []
for record in reader:
    line_following_dat.append(record)

# Read the barrier synthetic dataset
collision_file = open('barrier.dat', 'r')
reader = csv.reader(collision_file)
collision_dat = []
for record in reader:
    collision_dat.append(record)

# Combined graph data variables
graph_width  = 800
graph_height = 400
graph_bl     = (0, 0)
graph_tr     = (len(collision_dat), len(collision_dat))
pad_xy       = (0, 10)

# Create the frist header which will control input values
buttons1    = [sg.Button('Barrier 1'), sg.Button('Line 1'), sg.Button('Collision 1')]
start_time1 = [sg.Text('Start Time 1 (hh:mm):'), sg.In(size=5)]
end_time1   = [sg.Text('End Time 1 (hh:mm):'), sg.In(size=5)]
header1     = buttons1 + start_time1 + end_time1

# Create the first graph which will display a single dataset
graph1 = [sg.Canvas(size=(graph_width, graph_height), key='-CANVAS-')]

# Create the second header
# Create the frist header which will control input values
# buttons2    = [sg.Button('Barrier 2'), sg.Button('Line 2'), sg.Button('Collision 2')]
# start_time2 = [sg.Text('Start Time 2 (hh:mm):'), sg.In(size=5)]
# end_time2   = [sg.Text('End Time 2 (hh:mm):'), sg.In(size=5)]
# header2     = buttons2 + start_time2 + end_time2

# Create the first graph which will display a single dataset
#graph2 = [sg.Canvas(size=(graph_width, graph_height), key='-CANVAS-')]

# All the stuff inside your window.
layout = [ [sg.Text("MakeBlock Mission Data", expand_x=True, justification='center', font=('Arial', 30), background_color='Black')],
           [sg.Text("Sensor 1 Readings:", font=('Arial', 20), background_color="Black", pad=10)],
           header1, graph1 ]
        #    [sg.Text("Sensor 2 Readings:", font=('Arial', 20), background_color="Black", pad=10)],
        #    header2, graph2 ]

flag = False
# Function to create the graph based on the selected button
def create_graph(button_num):
    global flag
    print(flag)
    if flag == True:
        ax.clear()
    # Data for the graph (You can replace this with your own data)
    x = np.linspace(1, 12, 12)
    
    if button_num == 1:
        # Display Barrier Data
        flag = True
        data_type = 'Barrier Sensors'
        y1 = [0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0]
        y2 = [0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0]
        y3 = [0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0]
        fig, ax = plt.subplots(figsize=(5, 4))
        ax.plot(x, y1, marker='o', color='b', label='Line 1')
        ax.plot(x, y2, marker='s', color='r', label='Line 2')
        ax.plot(x, y3, marker='^', color='g', label='Line 3')
    elif button_num == 2:
        # Display Line Data
        flag = True
        data_type = 'Line Following Sensors'
        y1 = [1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0]
        y2 = [1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1]
        fig, ax = plt.subplots(figsize=(5, 4))
        ax.plot(x, y1, marker='o', color='b', label='Line 1')
        ax.plot(x, y2, marker='s', color='r', label='Line 2')
    elif button_num == 3:
        # Display Line Data
        flag = True
        data_type = 'Collision Sensors'
        y1 = [0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0]
        y2 = [0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0]
        fig, ax = plt.subplots(figsize=(5, 4))
        ax.plot(x, y1, marker='o', color='b', label='Line 1')
        ax.plot(x, y2, marker='s', color='r', label='Line 2')

    # Create the figure and plot the graph
    ax.set_xlabel('Time (Seconds)', fontsize=10)
    ax.set_ylabel('Sensor Reading', fontsize=10)
    ax.set_title(f'Line graph for the {data_type}', fontsize=9)
    ax.tick_params(axis='x', labelsize=6)  # Set the font size for the x-axis tick labels
    ax.tick_params(axis='y', labelsize=6) 
    ax.grid(True)
    ax.legend()
    
    # Set x-axis ticks to show each number from 1 through 12
    ax.set_xticks(x)

    # Clear previous drawings on the Matplotlib element
    canvas_elem.TKCanvas.delete("all")

    # Draw the figure on the Matplotlib element
    figure_canvas_agg = FigureCanvasTkAgg(fig, master=canvas_elem.TKCanvas)
    figure_canvas_agg.draw()
    figure_canvas_agg.get_tk_widget().pack(side='top', fill='both', expand=1)
        

# Create the Window
sg.theme('Black')
window = sg.Window('Mission Data', layout, resizable=True, margins=(20, 20))

# Get the Matplotlib element from the layout
canvas_elem = window['-CANVAS-']

# Event Loop to process "events" and get the "values" of the inputs
while True:
    event, values = window.read()
    if event == 'Barrier 1':
        create_graph(1)
    if event == 'Line 1':
        create_graph(2)
    if event == 'Collision 1':
        create_graph(3)
    if event == sg.WIN_CLOSED:
        break

window.close()
