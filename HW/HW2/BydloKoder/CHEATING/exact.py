import matplotlib.pyplot as plt

with open('out.txt', 'r') as infile:
    for line in infile:
        data = line.split(',')
data_right = []
for i in range(len(data)):
    if data[i].isnumeric():
        data_right.append(float(data[i]))
#print(data_right)

count = 0
data_to_plot = [[0 for j in range(1033)] for i in range(1033)]
for i in range(len(data_right)):
    point = data_right[i]
    data_to_plot[int(point/1033)][int(point%1033)] = 1
    count += 1


print(count)
plt.imshow(data_to_plot)
plt.show()



