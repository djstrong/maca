#ifndef LIBPLTAGGER_TAGSETCONVERTER_H
#define LIBPLTAGGER_TAGSETCONVERTER_H

#include <libpltagger/tagset.h>
#include <libpltagger/token.h>
#include <boost/function.hpp>

namespace PlTagger {

	class TagsetConverter
	{
	public:
		TagsetConverter(const Tagset& from, const Tagset& to);

		virtual ~TagsetConverter();

		virtual void convert_simple(const std::vector<Token*>& v,
				boost::function<void (Token*)> sink);

		virtual void convert_ambiguous(const std::vector< std::vector<Token*> >& v,
				boost::function<void (Token*)> sink);

		const Tagset tagset_from() const {
			return from_;
		}

		const Tagset tagset_to() const {
			return to_;
		}

	protected:
		static size_t find_shortest(const std::vector< std::vector<Token*> >& v,
				size_t& min_len_path);

	private:
		const Tagset& from_;

		const Tagset& to_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_TAGSETCONVERTER_H
