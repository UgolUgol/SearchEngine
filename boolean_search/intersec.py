
def intersection(lst1, lst2): 
    lst3 = [value for value in lst1 if value in lst2] 
    return lst3 

a = []
with open('file1') as f1:
	for line in f1:
		a.append(int(line))
b = []
with open('file2') as f2:
	for line in f2:
		b.append(int(line))

s = set(intersection(a, b))
print(s)

