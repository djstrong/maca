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

#include <libmaca/io/text.h>
#include <libpwrutils/foreach.h>

namespace Maca {

TextReader::TextReader(std::istream &is,
		const boost::shared_ptr<SentenceAnalyser>& sa)
	: Corpus2::BufferedSentenceReader(sa->tagset()), is_(is), sa_(sa)
{
	sa_->set_input_source(is);
}

Corpus2::Sentence::Ptr TextReader::actual_next_sentence()
{
	return sa_->get_next_sentence();
}


} /* end ns Maca */
