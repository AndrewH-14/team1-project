import PySimpleGUI as sg
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np
import io

# Create the frist header which will control input values
buttons1 = [sg.Button('Barrier', font=('Arial', 16)), sg.Button('Line', font=('Arial', 16)), sg.Button('Collision', font=('Arial', 16))]
start_time1 = [sg.Text('Start Time 1 (hh:mm):', font=('Arial', 14)), sg.In(size=8)]
end_time1 = [sg.Text('End Time 1 (hh:mm):', font=('Arial', 14)), sg.In(size=8)]
header1 = buttons1 + start_time1 + end_time1

# Create the first graph which will display a single dataset
graph1 = [sg.Image(filename='', key='-IMAGE-')]

# All the stuff inside your window.
layout = [[sg.Text("MakeBlock Mission Data", expand_x=True, justification='center', font=('Arial', 30), background_color='Black')],
          [sg.Text("Sensor Readings:", font=('Arial', 20), background_color="Black", pad=10)],
          header1, graph1]

# Create the Window
sg.theme('Black')
window = sg.Window('Mission Data', layout, resizable=True, margins=(20, 20), finalize=True)  # Add finalize=True parameter

# Function to update the graph based on the selected button
def update_graph(button_num):
    # Data for the graph (You can replace this with your own data)
    x = np.linspace(1, 12, 12)

    if button_num == 1:
        # Display Barrier Data
        data_type = 'Barrier Sensors'
        y1 = [0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0]
        y2 = [0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0]
        y3 = [0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0]
        labels = ['Sensor 1', 'Sensor 2', 'Sensor 3']
    elif button_num == 2:
        # Display Line Data
        data_type = 'Line Following Sensors'
        y1 = [1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0]
        y2 = [0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1]
        labels = ['Sensor 1', 'Sensor 2']
    elif button_num == 3:
        # Display Line Data
        data_type = 'Collision Sensors'
        y1 = [0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0]
        y2 = [0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0]
        labels = ['Sensor 1', 'Sensor 2']

    # Create the figure and plot the graph
    fig, ax = plt.subplots(figsize=(10, 7))  # Change the figsize to (8, 6)
    ax.plot(x, y1, marker='o', color='b', label='Line 1')
    ax.plot(x, y2, marker='s', color='r', label='Line 2')

    if button_num == 1:
        ax.plot(x, y3, marker='^', color='g', label='Line 3')

    ax.set_xlabel('Time (Seconds)', fontsize=14)
    ax.set_ylabel('Sensor Reading', fontsize=14)
    ax.set_title(f'Line graph for the {data_type}', fontsize=16)
    ax.tick_params(axis='x', labelsize=12)
    ax.tick_params(axis='y', labelsize=12)
    ax.grid(True)
    ax.legend(labels, title='Legend')
    ax.set_xticks(x)

    # Convert the Matplotlib figure to an image
    figure_to_image(fig, '-IMAGE-')

def figure_to_image(fig, key):
    # Convert a Matplotlib figure to an image
    buf = io.BytesIO()
    fig.savefig(buf, format='png')
    buf.seek(0)
    window[key].update(data=buf.getvalue())

# Event Loop to process "events" and get the "values" of the inputs
while True:
    event, values = window.read()
    if event == sg.WIN_CLOSED:
        break
    if event == 'Barrier':
        update_graph(1)
    if event == 'Line':
        update_graph(2)
    if event == 'Collision':
        update_graph(3)

window.close()
