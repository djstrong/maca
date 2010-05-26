#!/usr/bin/env python

import sys
import codecs

if len(sys.argv) < 3:
	print """
tab-to-sfst.py: convert whitespace-delimited data file to SFST format.
Lines in the input file are expected to be in the following format:
orth	lemma	tag

Usage: tab-to-sfst.py infile outfile

Progress info is output to stdout every 1000 lines processed.
"""
	exit()


file = codecs.open(sys.argv[1], 'r', 'utf8')
ofile = codecs.open(sys.argv[2], 'w', 'utf8')

escapes = []
for e in ('\\', ':', '-', '+', '.'):
	escapes.append((e, '\\' + e))
	
def ewrite(c):
	for e1, e2 in escapes:
		if c == e1:
			c = e2
	ofile.write(c)

li = 0
for line in file:
	orth, lemma, tag = line.split()
	for e1, e2 in escapes:
		tag = tag.replace(e1, e2)
	i = 0
	m = min(len(orth), len(lemma))
	while (i < m):
		co = orth[i]
		cl = lemma[i]
		if co == cl:
			ewrite(co)
		else:
			ewrite(cl)
			ofile.write(':')
			ewrite(co)
		i = i + 1
	while (i < len(orth)):
		co = orth[i]
		ofile.write('<>:')
		ewrite(co)
		i = i + 1
	while (i < len(lemma)):
		cl = lemma[i]
		ewrite(cl)
		ofile.write(':<>')
		i = i + 1
	ofile.write('<' + tag + '>:<>\n')
	li = li + 1
	if li % 1000 == 0:
		print "\r", li,
