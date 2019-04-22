import re
import operator
import math
import matplotlib.pyplot as plt
regexpr1 = re.compile("^.+\\|https.+\\|.+")
regexpr2 = re.compile(".+_.+")
def badTokensFilter(token):
	if regexpr1.match(token) or token == "@dummy" or regexpr2.match(token):
		return False
	return True

data = []
with open('tokens') as file :
	next(file)
	data = file.readlines()

filtered = list(filter(badTokensFilter, data))

counter_dict = {}
for token in filtered:
	if token in counter_dict:
		counter_dict[token] += 1
	else:
		counter_dict[token] = 1

sortedTokens = sorted(counter_dict.items(), key = lambda kv:(kv[1], kv[0]), reverse=True) 
X = []
Y = []
for idx, (key, val) in enumerate(sortedTokens):
	X.append(idx)
	Y.append(math.log(val))

plt.plot(X, Y)
plt.show()
