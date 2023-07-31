import PySimpleGUI as sg
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np

# Function to create the graph based on the selected button
def create_graph(button_num):
    # Data for the graph (You can replace this with your own data)
    x = np.linspace(0, 12, 100)
    y = np.sin(button_num * x)

    # Create the figure and plot the graph
    fig, ax = plt.subplots()
    ax.plot(x, y)
    ax.set_xlabel('X-axis')
    ax.set_ylabel('Y-axis')
    ax.set_title(f'Line graph for Button {button_num}')
    ax.grid(True)

    # Clear previous drawings on the Matplotlib element
    canvas_elem.TKCanvas.delete("all")

    # Draw the figure on the Matplotlib element
    figure_canvas_agg = FigureCanvasTkAgg(fig, master=canvas_elem.TKCanvas)
    figure_canvas_agg.draw()
    figure_canvas_agg.get_tk_widget().pack(side='top', fill='both', expand=1)

# Define the layout for the GUI
layout = [
    [sg.Button('Button 1'), sg.Button('Button 2'), sg.Button('Button 3')],
    [sg.Canvas(size=(400, 300), key='-CANVAS-')],
]

# Create the window
window = sg.Window('Line Graph with PySimpleGUI', layout)

# Get the Matplotlib element from the layout
canvas_elem = window['-CANVAS-']

# Event loop
while True:
    event, values = window.read()

    if event == sg.WINDOW_CLOSED:
        break
    elif event == 'Button 1':
        create_graph(1)
    elif event == 'Button 2':
        create_graph(2)
    elif event == 'Button 3':
        create_graph(3)

# Close the window and exit the GUI
window.close()