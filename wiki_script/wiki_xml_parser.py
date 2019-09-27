import mwxml
from progress.bar import Bar

dump = mwxml.Dump.from_file(open("wiki_dump"))


full_size = 11948782663
current_size = 0

f = open('res1_url', 'w')
print(full_size, file=f)
bar = Bar('Processing', max=full_size)
for page in dump:
	if page.id == 4:
		continue

	title = page.title
	url = "https://ru.wikipedia.org/wiki/"+title
	for revision in page:
		if revision.text == None:
			continue
		size = len(revision.text) + 1
		print("WIKIPEDIA_ARTICLE_BEGIN: ", page.title, '|WIKI_URL: ', url, "|", size, file=f)
		print(revision.text, file=f)
		bar.next(size)
		

f.close()

