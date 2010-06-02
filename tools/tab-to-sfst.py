#!/usr/bin/env python

import sys
import codecs
import timeit

if len(sys.argv) < 3:
	print """
tab-to-sfst.py: convert whitespace-delimited data file to SFST format.
Lines in the input file are expected to be in the following format:
orth	lemma	tag

Usage: tab-to-sfst.py infile outfile

Progress info is output to stdout every 1000 lines processed.
"""
	exit()


def work():
	file = codecs.open(sys.argv[1], 'r', 'utf8')
	ofile = codecs.open(sys.argv[2], 'w', 'utf8')
	escapes = []
	for e in ('\\', ':', '-', '+', '.'):
		escapes.append((e, '\\' + e))
		
	def ewrite(olist, c):
		if c == '\\':
			c = '\\\\'
		elif c == ':':
			c = '\\:'
		elif c == '-':
			c = '\\-'
		elif c == '+':
			c = '\\+'
		elif c == '.':
			c = '\\.'
		#for e1, e2 in escapes:
		#	if c == e1:
		#		c = e2
		olist.append(c)
	zzlist = []
	li = 0
	for line in file:
		olist = []
		orth, lemma, tag = line.split()
		for e1, e2 in escapes:
			tag = tag.replace(e1, e2)
		i = 0
		m = min(len(orth), len(lemma))
		while (i < m):
			co = orth[i]
			cl = lemma[i]
			if co == cl:
				ewrite(olist, co)
			else:
				ewrite(olist, cl)
				olist.append(':')
				ewrite(olist, co)
			i = i + 1
		while (i < len(orth)):
			co = orth[i]
			olist.append('<>:')
			ewrite(olist, co)
			i = i + 1
		while (i < len(lemma)):
			cl = lemma[i]
			ewrite(olist, cl)
			olist.append(':<>')
			i = i + 1
		olist.append('<')
		olist.append(tag)
		olist.append('>:<>\n')
		li = li + 1
		if li % 1000 == 0:
			print "\r", li,
			zzlist.append(''.join(olist))
			zzlist = []
	ofile.write(''.join(zzlist))

t = timeit.Timer(work)

print t.timeit(1), "seconds elapsed"