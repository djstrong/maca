#ifndef LIBPLTAGGER_SENTENCE_H
#define LIBPLTAGGER_SENTENCE_H

#include <libpltagger/token.h>

#include <boost/range.hpp>

namespace PlTagger {

	class Sentence
	{
	public:
		template <typename T>
		Sentence(const T& range);

		size_t size() const {
			return tokens_.size();
		}

		Token* operator[](size_t idx) {
			return tokens_[idx];
		}

		const Token* operator[](size_t idx) const {
			return tokens_[idx];
		}

		const std::vector<Token*> tokens() const {
			return tokens_;
		}

	private:
		std::vector<Token*> tokens_;

	};

	template <typename T>
	Sentence::Sentence(const T &range)
		: tokens_()
	{
		std::copy(range.begin(), range.end(), std::back_inserter(tokens_));
	}

} /* end ns PlTagger */

#endif // LIBPLTAGGER_SENTENCE_H
