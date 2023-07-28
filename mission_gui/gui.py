import PySimpleGUI as sg
import csv

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
graph_width  = 1000
graph_height = 300
graph_bl     = (0, 0)
graph_tr     = (len(collision_dat), len(collision_dat))
pad_xy       = (0, 10)

# Create the frist header which will control input values
buttons1    = [sg.Button('Barrier 1'), sg.Button('Line 1'), sg.Button('Collision 2')]
start_time1 = [sg.Text('Start Time 1 (hh:mm):'), sg.In(size=5)]
end_time1   = [sg.Text('End Time 1 (hh:mm):'), sg.In(size=5)]
header1     = buttons1 + start_time1 + end_time1

# Create the first graph which will display a single dataset
graph1 = [sg.Graph(canvas_size=(graph_width, graph_height), graph_bottom_left=graph_bl, graph_top_right=graph_tr, pad=pad_xy, expand_x=True)]

# Create the second header
# Create the frist header which will control input values
buttons2    = [sg.Button('Barrier 2'), sg.Button('Line 2'), sg.Button('Collision 2')]
start_time2 = [sg.Text('Start Time 2 (hh:mm):'), sg.In(size=5)]
end_time2   = [sg.Text('End Time 2 (hh:mm):'), sg.In(size=5)]
header2     = buttons2 + start_time2 + end_time2

# Create the first graph which will display a single dataset
graph2 = [sg.Graph(canvas_size=(graph_width, graph_height), graph_bottom_left=graph_bl, graph_top_right=graph_tr, pad=pad_xy, expand_x=True)]

# All the stuff inside your window.
layout = [ [sg.Text("MakeBlock Mission Data", expand_x=True, justification='center', font=('Arial', 30), background_color='Black')],
           [sg.Text("Sensor 1 Readings:", font=('Arial', 17), background_color="Black", pad=10)],
           header1, graph1,
           [sg.Text("Sensor 2 Readings:", font=('Arial', 17), background_color="Black", pad=10)],
           header2, graph2 ]

# Create the Window
sg.theme('Black')
window = sg.Window('Mission Data', layout, resizable=True, margins=(20, 20))
# Event Loop to process "events" and get the "values" of the inputs
while True:
    event, values = window.read()
    if event == 'Barrier':
        print("Display Barrier Data!")
    if event == 'Line':
        print("Display Line Data!")
    if event == 'Collision':
        print("Display Collision Data!")
    if event == sg.WIN_CLOSED:
        break

window.close()
