#!/usr/bin/python
# -*- coding: utf-8 -*-

import ctypes, sys
sys.setdlopenflags(sys.getdlopenflags() | ctypes.RTLD_GLOBAL)

import maca
import corpus2
from optparse import OptionParser

descr = """%prog MACA_CONFIG_FILE CORPUS_FILE [options]
Reads a corpus file and outputs all or some tokens.
Available input formats: plain premorph
Available output formats: """ + ' '.join(corpus2.TokenWriter.available_writer_types()) + """
""" + ' '.join(corpus2.TokenWriter.available_writer_types_help())

def sentences(reader):
	"""Yields subsequent sentences from a reader."""
	while True:
		sentence = reader.get_next_sentence()
		if not sentence:
			break
		yield sentence

def chunks(reader):
	"""Yields subsequent sentences from a reader."""
	while True:
		chunk = reader.get_next_chunk()
		if not chunk:
			break
		yield chunk

def maca_analyse():
	parser = OptionParser(usage=descr)
	parser.add_option('-i', '--input-format', type='string', action='store',
		dest='input_format', default='plain',
		help='set the input format; available formats: plain, premorph')
	parser.add_option('-o', '--output-format', type='string', action='store',
		dest='output_format', default='xces',
		help='set the output format; default: xces')
	parser.add_option('-s', action='store_true', default=False, dest='split', help='Split output into chunks on many-newline tokens')
	(options, args) = parser.parse_args()

	if len(args) != 2:
		print "MACA_CONFIG_FILE and CORPUS_FILEPATH are required"
		return

	maca_config = args[0]
	filepath = args[1]

	reader = ''
	if options.input_format == 'plain':
		reader = maca.TextFileReader.create_reader(filepath, maca_config)
	elif options.input_format == 'premorph':
		reader = maca.PremorphFileReader.create_reader(filepath, maca_config)
	else:
		print "Unknown input format."
		return
	
	writer = corpus2.TokenWriter.create_stdout_writer(options.output_format, reader.tagset())

	if options.split:
		for chunk in chunks(reader):
			writer.write_chunk(chunk)
	else:
		for sentence in sentences(reader):
			writer.write_sentence(sentence)
		
	

if __name__ == '__main__':
	maca_analyse()