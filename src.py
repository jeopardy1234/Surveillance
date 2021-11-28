from onem2m import *
uri_cse = "http://localhost:8080/~/in-cse/in-name"
ae  = "Ultrasonic_Sensor"
cnt = "Distance"

create_ae(uri_cse, ae)
uri_ae = uri_cse + "/" + ae
create_cnt(uri_ae, cnt)
uri_cnt = uri_ae + "/" + cnt

# delete_ae("http://localhost:8080/~/in-cse/in-name/Water-Level-Sensor")
