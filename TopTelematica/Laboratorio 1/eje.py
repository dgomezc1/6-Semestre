import re
exp_reg_URL = re.compile('([\w]+)://([\w+\.]+)([/\w|-]+).(\w+)') 

s = "https://cdn.pixabay.com/photo/2017/11/13/22/12/compass-2946959_960_720.jpg"
exp_reg_URL1 = re.findall('([\w]+)://([\w+\.]+)([/\w|-]+).(\w+)',s) 
if exp_reg_URL.match(s):
    print("melo mi fafa")
print(exp_reg_URL1)