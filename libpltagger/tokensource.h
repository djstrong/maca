#ifndef LIBPLTAGGER_TOKEN_SOURCE_H
#define LIBPLTAGGER_TOKEN_SOURCE_H

#include <boost/range.hpp>
#include <iostream>

namespace PlTagger {

	/// forward declaration
	class Token;

	/**
	 * Simple interface for classes that are tagger token sources
	 */
	class TokenSource
	{
	public:
		virtual ~TokenSource();

		virtual Token* get_next_token() = 0;
	};

	/**
	 * Generic TokenSource wrapper for containers of tagger Token pointers,
	 * e.g. a std::vector<Token*> or a boost::range of Token* iterators.
	 *
	 * The container should not be modified as long as it is being accesed
	 * by a RangeSource wrapper.
	 */
	template<typename T>
	class RangeSource : public TokenSource
	{
	public:
		typedef typename T::const_iterator const_iterator;

		RangeSource(const T& range)
			: end_(range.end()), ptr_(range.begin())
		{
		}

		Token* get_next_token()
		{
			if (ptr_ != end_) {
				return *ptr_++;
			} else {
				return NULL;
			}
		}

	private:
		const_iterator end_;

		const_iterator ptr_;
	};

	template<class T>
	RangeSource<T>* make_range_source(const T& range)
	{
		return new RangeSource<T>(range);
	}

	template<class T>
	RangeSource<boost::iterator_range<T> >* make_range_source(const T& b, const T& e)
	{
		boost::iterator_range<T> range(b, e);
		return new RangeSource< boost::iterator_range<T> >(range);
	}

} /* end ns PlTagger */

#endif // LIBPLTAGGER_TOKEN_SOURCE_H
