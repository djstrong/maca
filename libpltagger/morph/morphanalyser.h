#ifndef LIBPLTAGGER_MORPHANALYSER_H
#define LIBPLTAGGER_MORPHANALYSER_H

#include <libtoki/token.h>
#include <libpltagger/confignode.h>
#include <libpltagger/token.h>
#include <libpltagger/tagset.h>

#include <boost/function.hpp>

namespace PlTagger {

	class MorphAnalyser
	{
	public:
		explicit MorphAnalyser(const Tagset* tagset);

		explicit MorphAnalyser(const Config::Node& cfg);

		virtual ~MorphAnalyser();

		std::vector<Token*> process(const Toki::Token& t);

		void process(const Toki::Token &t, std::vector<Token*>& vec);

		virtual void process_functional(const Toki::Token &t, boost::function<void (Token*)> sink) = 0;

		const Tagset& tagset() const {
			return *tagset_;
		}

		void set_tagset(const Tagset* tagset) {
			tagset_ = tagset;
		}

		void parse_tag_into_token(Token* tok, const UnicodeString& lemma,
				const std::string& tag) const;

	private:
		const Tagset* tagset_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_MORPHANALYSER_H
