
import socket
import matplotlib.pyplot as plt

import requests
import json
import plotly.express as px
import pandas as pd

headers = {
    'X-M2M-Origin': 'admin:admin',
    'Content-type': 'application/json'
}

uri_cnt = "http://127.0.0.1:8080/~/in-cse/in-name/ultrasonic_sensor_1/distance"
# getting all content instances under cnt
uri_cnt += '/?rcn=4'


# Define socket host and port
SERVER_HOST = '0.0.0.0'
SERVER_PORT = 8000

# Create socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_socket.bind((SERVER_HOST, SERVER_PORT))
server_socket.listen(1)
print('Listening on port %s ...' % SERVER_PORT)
reload_string = """
<script>
    function autoRefresh() {
        window.location = window.location.href;
    }
    setInterval('autoRefresh()', 5000);
</script>
"""
string_html = ''
var = 0

while True:
    # get values from onem2m
    # get request to get all content instances
    # response = requests.get(uri_cnt, headers=headers)
    # ret_data = json.loads(response.text)

    # make radar graph
    var = var + 1
    df = pd.DataFrame(dict(
        r=[var, 5, 2, 2, 3],
        theta=['processing cost', 'mechanical properties', 'chemical stability',
               'thermal stability', 'device integration']))
    fig = px.line_polar(df, r='r', theta='theta', line_close=True)
    fig.write_html("radar_plotter.html")
    
    # read all content from radar_plotter.html
    with open("radar_plotter.html", "r") as f:
        string_html = f.read()
    string_html = string_html[0:37]+reload_string+string_html[37:]
    # Wait for client connections
    client_connection, client_address = server_socket.accept()
    # Get the client request
    request = client_connection.recv(1024).decode()
    # print(request)

    # Send HTTP response
    response = 'HTTP/1.0 200 OK\n\n'
    response += string_html
    client_connection.sendall(response.encode())
    # print(response.encode())
    client_connection.close()

# Close socket
server_socket.close()
