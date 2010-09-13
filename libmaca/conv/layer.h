#ifndef LIBMACA_TOKEN_LAYER_H
#define LIBMACA_TOKEN_LAYER_H

#include <libmaca/tokensource.h>
#include <libmaca/tagset.h>
#include <boost/function.hpp>
#include <vector>

namespace Maca { namespace Conversion {

	/**
	 * Base class for conversion layers.
	 *
	 * Layers accept tokens in one tagset, perform some changes, and output
	 * tokens in a possibly different tagset via the TokenSource interface.
	 */
	class Layer : public TokenSource
	{
	public:
		/// Constructor
		Layer();

		/// Destructor
		virtual ~Layer();

		/// Cloning
		virtual Layer* clone() const = 0;

		/// Source setter
		void set_source(TokenSource* src);

		/// Source getter
		TokenSource* source();

		/**
		 * The tagset of tokens coming into the layer.
		 */
		virtual const Tagset& tagset_from() const = 0;

		/**
		 * The tagset of tokens coming out of the layer.
		 */
		virtual const Tagset& tagset_to() const = 0;

	private:
		/// The current token source
		TokenSource* source_;
	};

	/**
	 * Base class for conversion layers that do not change a token's tagset.
	 *
	 * (i.e. the 'from' and 'to' tagsets are identical).
	 */
	class OneTagsetLayer : public Layer
	{
	public:
		/**
		 * Constructor.
		 * @param tagset the tagset used for tokens coming in and out of the
		 *               layer.
		 */
		OneTagsetLayer(const Tagset& tagset);

		/// Layer override
		const Tagset& tagset_from() const;

		/// Layer override
		const Tagset& tagset_to() const;

		/**
		 * Redundant tagset accesor for consistency and to avoid having to
		 * choose between _from and _to in derived classes.
		 */
		const Tagset& tagset() const {
			return tagset_;
		}

	private:
		/// The tagset
		const Tagset& tagset_;
	};

} /* end ns Conversion */ } /* end ns Maca */

#endif // LIBMACA_TOKEN_LAYER_H
