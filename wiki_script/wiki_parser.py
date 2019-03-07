import wikipediaapi
from multiprocessing.dummy import Pool as ThreadPool
from itertools import product
from threading import Thread, Lock
import os
import requests
from aiohttp import ClientSession
import json

import asyncio
import mwparserfromhell as mw
from aiohttp import ClientSession

async def fetch(url, name, session):
	async with session.get(url, params={'action': 'query', 'format': 'json', 'titles': name, 'prop': 'extracts', 'explaintext': 1}) as response:
		return (name, response.status, await response.text())

async def run(articlesList):
	url = "https://ru.wikipedia.org/w/api.php"
	tasks = []


	async with ClientSession() as session:
		for name in articlesList:
			task = asyncio.ensure_future(fetch(url, name, session))
			tasks.append(task)

		return await asyncio.gather(*tasks)




def downloadTexts(articles):

	wetTexts = []
	goodTexts = []
	badTexts = []
	loop = asyncio.get_event_loop()
	future = asyncio.ensure_future(run(articles))
	loop.run_until_complete(future)

	wetTexts = future.result()
	for (name, status, text) in wetTexts:
		if status == 200:
			goodTexts.append(text)
		else:
			badTexts.append(name)

	if len(badTexts) > 0:
		goodTexts = goodTexts + downloadTexts(badTexts)

	return goodTexts


def getArticlesList(categorymembers, level=0, max_level=2):
	articlesList = []
	for c in categorymembers.values():
		if c.ns == wikipediaapi.Namespace.CATEGORY and level < max_level:
			articlesList += getArticlesList(c.categorymembers, level=level + 1, max_level=max_level)
		if c.ns == wikipediaapi.Namespace.MAIN:
			articlesList.append(c.title)
	return articlesList

def applyRegex(text, regex):
	return regex.sub(' ', text).replace("/", "-")

def handleTexts(wetTexts):
	texts = []
	for text in wetTexts:

		try:
			page = next(iter(json.loads(text)['query']['pages'].values()))	
			title = page['title']
			#wikicode = page['revisions'][0]['*']
			#parsed_wikicode = mw.parse(wikicode)
			print("WIKIPEDIA_ARTICLE_BEGIN: ", title)
			#stripped_wikicode = parsed_wikicode.strip_code()
			wikitext = page['extract']
			print(wikitext)
			print("WIKIPEDIA_ARTICLE_END")
		except ValueError:
			print("[!!!] Pass none text: ")

	return texts


wiki_wiki = wikipediaapi.Wikipedia(language='ru', extract_format=wikipediaapi.ExtractFormat.WIKI)   
cat = wiki_wiki.page("Category: Искусство")

#print("Downloading...")
articles = getArticlesList(cat.categorymembers)

#print("Donwloading texts...")

texts = downloadTexts(articles)

# # #print("Handle texts...")
texts = handleTexts(texts)

