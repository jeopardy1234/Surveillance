# While this program is running, go to localhost:8080 to
# see the output
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

uri_cnt = "http://localhost:8080/~/in-cse/in-name/Radar/dist_"
# getting all content instances under cnt
# uri_cnt += '/?rcn=4'


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
    values = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    for i in range(7):
        new_uri = uri_cnt + str(i*30) + "_deg/la"
        response = requests.get(new_uri, headers=headers)
        ret_data = json.loads(response.text)
        values[i] = int(ret_data['m2m:cin']['con'])
        # print(response.text)
    
    # make radar graph
    var = var + 1
    df = pd.DataFrame(dict(
        r=values,
        theta=['0 degree', '30 degree', '60 degree', '90 degree', '120 degree', '150 degree', '180 degree', 'nil1', 'nil2', 'nil3', 'nil4', 'nil5']))
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
