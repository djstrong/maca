/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libmaca project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE and COPYING files for more details.
*/

#ifndef LIBMACA_IO_PREMORPH_H
#define LIBMACA_IO_PREMORPH_H

#include <libcorpus2/token.h>
#include <libcorpus2/io/reader.h>
#include <libmaca/morph/morphanalyser.h>

#include <libtoki/tokenizer/tokenizer.h>

#include <boost/scoped_ptr.hpp>

namespace Maca {

class PremorphProcessorImpl;

class SentenceAnalyser;

class PremorphProcessor
{
public:
	PremorphProcessor(std::ostream& os,
			const boost::shared_ptr<Toki::Tokenizer>& tok,
			const boost::shared_ptr<Maca::MorphAnalyser>& ma);

	PremorphProcessor(std::ostream& os,
			const boost::shared_ptr<SentenceAnalyser>& sa);

	~PremorphProcessor();

	void parse_file(const std::string& filename);

	void parse_stream(std::istream& is);

	void set_stats(bool v);

protected:
	boost::scoped_ptr<PremorphProcessorImpl> impl_;

	size_t tokens_;

	size_t sentences_;
};

class PremorphReaderImpl;

class PremorphReader : public Corpus2::BufferedChunkReader
{
public:
	PremorphReader(std::istream& is,
			const boost::shared_ptr<Toki::Tokenizer>& tok,
			const boost::shared_ptr<Maca::MorphAnalyser>& ma);

	PremorphReader(std::istream& is,
			const boost::shared_ptr<SentenceAnalyser>& sa);

	~PremorphReader();

	std::istream& is() {
		return is_;
	}

protected:
	std::istream& is_;

	void ensure_more();

	boost::scoped_ptr<PremorphReaderImpl> impl_;
};

} /* end ns Maca */

#endif // LIBMACA_IO_PREMORPH_H
