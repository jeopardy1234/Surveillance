from onem2m import *
uri_cse = "http://localhost:8080/~/in-cse/in-name"
ae = "Radar"
cnt = "Distance"

# create_ae(uri_cse, ae)
# uri_ae = uri_cse + "/" + ae
# create_cnt(uri_ae, cnt)
# uri_cnt = uri_ae + "/" + cnt


# delete_ae("http://localhost:8080/~/in-cse/in-name/Radar")
# create_ae(uri_cse, ae)
if __name__ == "__main__":
    for i in range(7):
        cnt = f"dist_{i*30}_deg"
        uri_ae = uri_cse + "/" + ae
        create_cnt(uri_ae, cnt)
        uri_cnt = uri_ae + "/" + cnt
        create_data_cin(uri_cnt, "0")
    # delete_ae("http://localhost:8080/~/in-cse/in-name/ultrasonic_sensor_1")
