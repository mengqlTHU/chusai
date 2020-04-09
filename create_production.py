# Create script for submit

f_local = open("main.cpp",'r')
local_string = f_local.read()

local_string = local_string.replace("../data/test_data.txt", "/data/test_data.txt")
local_string = local_string.replace("../data/myresult.txt", "/projects/student/result.txt")

f_prod = open("production.cpp","w")
f_prod.write(local_string)

f_local.close()
f_prod.close()