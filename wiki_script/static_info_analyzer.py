file = open("res1", "r")
text = file.read()
splitted_texts = text.split("WIKIPEDIA_ARTICLE_END")

symbCounts = 0
for part in splitted_texts:
	symbCounts+=len(part)
	print(len(part))

print("Average len: ", symbCounts/(len(splitted_texts)) - 1)
print("Texts count: ", len(splitted_texts) - 1)