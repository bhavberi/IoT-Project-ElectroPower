from onem2m import *
uri_cse = "http://localhost:8080/~/in-cse/in-name"
ae = "Water_Temp"
cnt = "node"

create_ae(uri_cse, ae)
uri_ae = uri_cse + "/" + ae
create_cnt(uri_ae, cnt)
uri_cnt = uri_ae + "/" + cnt
create_data_cin(uri_cnt, "random_value")

ae = "Water_Level"
cnt = "node"

create_ae(uri_cse, ae)
uri_ae = uri_cse + "/" + ae
create_cnt(uri_ae, cnt)
uri_cnt = uri_ae + "/" + cnt
create_data_cin(uri_cnt, "random_value")

