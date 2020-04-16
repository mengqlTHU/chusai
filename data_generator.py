import random

input_file = './data/1004812/test_data.txt'
out_file = 'test_data_choices_80.txt'

f = open(input_file,'r')
lines = f.readlines()
choices = random.sample(lines, int(len(lines)*0.8))
random.shuffle(choices)

f_out = open(out_file,'w')
for line in choices:
    f_out.write(line)

f.close()
f_out.close()