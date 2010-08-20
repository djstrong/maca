#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys, codecs
from collections import defaultdict as dd
from optparse import OptionParser

descr = """%prog [options] infile outfile

Utility to convert a morphological dictionary to lower case. Useful when having
created a dictionary and later decided to set the analyser to case-insensitive
mode. The script will also report the number of entries that are repeated when
case-insensitive (these reps will be removed). Unfortunately, it doesn't
understand tags, so implicit repetitions will not be spotted (e.g. subst:_:_:_
and subst:m1:pl:nom).

By default only lemmas are converted.

NOTE: this script loads all the entries into memory; it may take long if the
input is large.
"""

def entries(infname):
	f = codecs.open(infname, 'rb', 'utf-8')
	for line in f:
		line = line.strip()
		if line:
			entry = line.split() # form, lemma, tag
			assert len(entry) == 3, ('unexpected line format: %s' % line)
			yield entry
	f.close()

def to_file(outfname):
	def store(data):
		f = codecs.open(outfname, 'wb', 'utf-8')
		for entry in data:
			f.write(u'\t'.join(entry))
			f.write(u'\n')
		f.close()
	return store

def convert(infname, outfname, options):
	def xform(entry):
		form, lemma, tag = entry
		if options.lower_forms:
			form = form.lower()
		if options.lower_lemmas:
			lemma = lemma.lower()
		return form, lemma, tag
	all_entries = [e for e in entries(infname)]
	print 'Input list:\t%d entries' % len(all_entries)
	out_reps = map(xform, all_entries)
	out_set = set(out_reps)
	print 'Output list:\t%d entries' % len(out_set)
	lendiff = len(all_entries) - len(out_set)
	to_file(outfname)(sorted(out_set))
	if lendiff > 0:
		if options.verbose:
			reps = dd(int)
			for entry in out_reps:
				reps[entry] += 1
			print 'Duplicated entries:'
			for entry in reps:
				if reps[entry] > 1:
					print '%s\t%s\t%s' % entry
			

if __name__ == '__main__':
	parser = OptionParser(usage=descr)
	parser.add_option('-l', '--lower-lemmas', action='store_true', dest='lower_lemmas', default=False, help='also lowercase lemmas')
	parser.add_option('-s', '--suppres-forms', action='store_false', dest='lower_forms', default=True, help='leave forms unchanged')
	parser.add_option('-v', '--verbose', action='store_true', dest='verbose', default=False, help='write repeated forms')
	
	(options, args) = parser.parse_args()
	
	if len(args) != 2:
		print 'You need to provide input and output filenames'
		print
		parser.print_help()
		sys.exit(1)
	
	infname = args[0]
	outfname = args[1]
	
	convert(infname, outfname, options)
