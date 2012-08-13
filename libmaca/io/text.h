/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libmaca project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE.MACA, LICENSE.SFST, LICENSE.GUESSER, COPYING.LESSER and COPYING files for more details.
*/

#ifndef LIBMACA_IO_PLAIN_H
#define LIBMACA_IO_PLAIN_H

#include <libcorpus2/io/reader.h>
#include <libmaca/util/sentenceanalyser.h>
#include <boost/shared_ptr.hpp>
#include <boost/ref.hpp>

namespace Maca {

class TextReader : public Corpus2::BufferedSentenceReader
{
public:
	TextReader(std::istream& is,
			const boost::shared_ptr<SentenceAnalyser>& sa, int bufsize = 1000);

	std::istream& is() {
		return is_;
	}

protected:
	/// BufferedSentenceReader override
	Corpus2::Sentence::Ptr actual_next_sentence();

	std::istream& is_;

	boost::shared_ptr<SentenceAnalyser> sa_;
};

class PlainTextReader : public TextReader
{
public:
    PlainTextReader(boost::shared_ptr<std::istream> inputStream, const boost::shared_ptr<SentenceAnalyser>& sa);
    ~PlainTextReader();

    static boost::shared_ptr<Corpus2::TokenReader> create_file_reader(const std::string& filename, const std::string& config);
    static boost::shared_ptr<Corpus2::TokenReader> create_stream_reader(const std::string& config);
    static boost::shared_ptr<Corpus2::TokenReader> create_string_reader(const std::string& inputText, const std::string& config);

private:

    boost::shared_ptr<std::istream> inputFileStream;

};

} /* end ns Maca */

#endif // LIBMACA_IO_PLAIN_H
