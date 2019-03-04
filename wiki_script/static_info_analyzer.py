file = open("res", "r")
text = file.read()
splitted_texts = text.split("WIKIPEDIA_ARTICLE_END")

average = 0
for part in splitted_texts:
	average+=len(part)
	print(len(part))

print("Average len: ", average/(len(splitted_texts)) - 1)
print("Texts count: ", len(splitted_texts) - 1)