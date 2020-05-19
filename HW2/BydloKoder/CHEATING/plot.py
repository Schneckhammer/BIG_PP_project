import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
import numpy as np

power = []
num_points = []

with open("training.csv", 'r') as infile:
    for line in infile:
        data = list(map(float, line.split(',')))
        power.append(data[0])
        num_points.append(data[1])

#print(power, num_points)




fig = plt.figure()

ax = fig.add_subplot(1, 1, 1)

#ax.scatter(power, num_points, s = [2 for i in range(len(power))])
ax.plot(power, num_points)

#plt.show()


def fn(x, *args):
    for i in range(25):
        if x < 2.01 + i / 25:
            #print(i)
            arg = args[i*2:(i+1)*2]
            return fn_abstract(x, *arg)





def fn_abstract(x, *args):
    res = 0
    for arg in args:
        res = res*x + arg
    return res



#def fn(x, a, b, c, d, e, f, g, h, i, j, k, l, m, pre_pos, pre_neg, ins_pos,
#       ins_neg, shift_pos, shift_neg):
#    return (a*x**8 + b*x**7 + c * x**6 + d * x**5 + e * x**4 + f * x ** 3 + g *
#x ** 2 + h * x**1 + i * x + j
#            pre_pos*np.exp(x-shift_pos)+pre_neg*np.exp(x-shift_neg))

guess = []
for i in range(2):
    guess.append(1)
params = []
#print(len(power[3*50:4*50]))
for i in range(25):
    params.extend(list(map(int, curve_fit(fn_abstract, power[(i*40):((i+1)*40)],
                            num_points[(i*40):(i+1)*40], guess)[0])))
#print(params[70:75])
#print(fn(2.145, *params))



#params = curve_fit(fn_abstract, power, num_points, guess)[0]
#print(params)
#print(len(params))
fitted = []
for i in range(len(power)):
    fitted.append(fn(power[i], *params))
#print(fitted)
ax.plot(power, fitted)
#plt.show()


string = "int fit_params[50] = {"
for i in range(49):
    string += str(params[i]) + ','

string += str(params[49]) + '};'
with open('fit_params', 'w') as outfile:
    outfile.write(string)








