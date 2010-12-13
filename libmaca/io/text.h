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

#ifndef LIBMACA_IO_PLAIN_H
#define LIBMACA_IO_PLAIN_H

#include <libcorpus2/io/reader.h>
#include <libmaca/util/sentenceanalyser.h>

namespace Maca {

class TextReader : public Corpus2::BufferedSentenceReader
{
public:
	TextReader(std::istream& is,
			const boost::shared_ptr<SentenceAnalyser>& sa);

	std::istream& is() {
		return is_;
	}

protected:
	/// BufferedSentenceReader override
	Corpus2::Sentence::Ptr actual_next_sentence();

	std::istream& is_;

	boost::shared_ptr<SentenceAnalyser> sa_;
};

} /* end ns Maca */

#endif // LIBMACA_IO_PLAIN_H
