#ifndef LIBPLTAGGER_TOKEN_LAYER_H
#define LIBPLTAGGER_TOKEN_LAYER_H

#include <libpltagger/tokensource.h>
#include <libpltagger/tagset.h>
#include <boost/function.hpp>
#include <vector>

namespace PlTagger { namespace Conversion {

	class Layer : public TokenSource
	{
	public:
		Layer();

		virtual ~Layer();

		void set_source(TokenSource* src);

		TokenSource* source();

		virtual const Tagset& tagset_from() const = 0;

		virtual const Tagset& tagset_to() const = 0;

	private:
		TokenSource* source_;
	};

	class OneTagsetLayer : public Layer
	{
	public:
		OneTagsetLayer(const Tagset& tagset);

		const Tagset& tagset_from() const;

		const Tagset& tagset_to() const;

		/**
		 * Redundant tagset accesor for consistency and to avoid having to
		 * choose between _from and _to in derived classes.
		 */
		const Tagset& tagset() const {
			return tagset_;
		}

	private:
		const Tagset& tagset_;
	};

} /* end ns Conversion */ } /* end ns PlTagger */

#endif // LIBPLTAGGER_TOKEN_LAYER_H
