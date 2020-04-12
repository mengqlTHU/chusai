# Create script for submit

f_local = open("main.cpp",'r', encoding="utf8")
# f_header = open("CircuitFinder.h",'r')
local_string = f_local.read()

# local_string = local_string.replace("../data/test_data.txt", "/data/test_data.txt")
# local_string = local_string.replace("../data/myresult.txt", "/projects/student/result.txt")
# local_string = local_string.replace('#include "CircuitFinder.h"', "")
local_string = local_string.replace('#define TEST', "")
local_string = local_string.replace('#define MYTIME', "")

f_prod = open("production.cpp","w",encoding="utf8")
f_prod.write(local_string)

f_local.close()
f_prod.close()