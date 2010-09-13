#ifndef LIBMACA_IO_READER_H
#define LIBMACA_IO_READER_H

#include <libmaca/chunk.h>
#include <libmaca/tokensource.h>
#include <deque>
#include <loki/Factory.h>
#include <loki/Singleton.h>

namespace Maca {

	/**
	 * Base class for readers of Maca tokens. Tokens are always read from a
	 * source and always within a given tagset.
	 *
	 * Tokens can be read on a token-by-token basis, or in entire senteces, or
	 * in entire chunks (paragraphs). Behavior is not defined if there are
	 * mixed calls to differend forms of getting data.
	 */
	class TokenReader : public TokenSource
	{
	public:
		TokenReader(std::istream& is, const Tagset& tagset);

		virtual ~TokenReader();

		virtual Token* get_next_token() = 0;

		virtual Sentence* get_next_sentence() = 0;

		virtual Chunk* get_next_chunk() = 0;

		std::istream& is() {
			return is_;
		}

		const Tagset& tagset() {
			return tagset_;
		}

		/**
		 * Factory interface for creating readers from string identifiers
		 *
		 * Mostly a convenience function to avoid having client code refer
		 * directly to the factory instance.
		 *
		 * @param class_id the unique class identifier
		 * @param os the output stream to pass to the reader's constructor
		 * @param tagset the tagset to pass to the reader's constructor
		 */
		static TokenReader* create(const std::string class_id,
				std::ostream& os,
				const Tagset& tagset,
				const string_range_vector& params);

		/**
		 * Factory interface for creating readers from string identifiers.
		 *
		 * Params are split from the class id and then the more general version
		 * is called. Parameters are expected to be comma-separated from the
		 * class id.
		 */
		static TokenReader* create(const std::string class_id_params,
				std::ostream& os,
				const Tagset& tagset);

		/**
		 * Function to get a vector of available reader type strings.
		 */
		static std::vector<std::string> available_reader_types();

		/**
		 * Function to get a vector of available reader type strings with help
		 * strings appended
		 */
		static std::vector<std::string> available_reader_types_help();

		/**
		 * Convenience template for registering TokenWriter derived classes.
		 */
		template <typename T>
		static bool register_reader(const std::string& class_id,
				const std::string& help = "");
	private:
		std::istream& is_;

		const Tagset& tagset_;
	};

	typedef Loki::Factory<
		TokenReader, // The base class for objects created in the factory
		std::string, // Identifier type
		Loki::TL::MakeTypelist<
			std::ostream&, const Tagset&, const string_range_vector&
		>::Result // TokenLayer constructor arguments' types specification
	> TokenReaderFactoryType;

	struct TokenReaderFactory
	{
		TokenReaderFactoryType factory;
		std::map<std::string, std::string> help;
	};

	/**
	 * Declaration of the TokenWriter factory as a singleton Loki object
	 * factory. The factory instance can be accessed as
	 * TokenLayerFactory::Instance(). It is assumed that all derived classes
	 * have the same constructor signature.
	 */
	typedef Loki::SingletonHolder<
		TokenReaderFactory,
		Loki::CreateUsingNew, // default, needed to change the item below
		Loki::LongevityLifetime::DieAsSmallObjectChild // per libloki docs
	>
	TokenReaderFactorySingleton;

	/**
	 * Convenience typedef for the exception type the factory throws
	 */
	typedef Loki::DefaultFactoryError<
		std::string, TokenReader
	>::Exception
	TokenReaderFactoryException;

	/**
	 * Convenience template TokenWriter creation function
	 */
	template <typename T>
	inline
	T* reader_creator(std::ostream& os, const Tagset& tagset,
			const string_range_vector& params)
	{
		return new T(os, tagset, params);
	}

	template <typename T>
	bool TokenReader::register_reader(const std::string& class_id,
			const std::string& help)
	{
		bool ret = TokenReaderFactorySingleton::Instance().factory.Register(
				class_id, reader_creator<T>);
		if (ret) {
			TokenReaderFactorySingleton::Instance().help[class_id] = help;
		}
		return ret;
	}

	/**
	 * Convenience class for readers that keep a buffer of chunks
	 */
	class BufferedTokenReader : public TokenReader
	{
	public:
		BufferedTokenReader(std::istream& is, const Tagset& tagset);

		~BufferedTokenReader();

		Token* get_next_token();

		Sentence* get_next_sentence();

		Chunk* get_next_chunk();

	protected:
		virtual void ensure_more() = 0;

		std::deque<Chunk*> chunk_buf_;
		std::deque<Sentence*> sentence_buf_;
		std::deque<Token*> token_buf_;
	};

} /* end ns Maca */

#endif // LIBMACA_IO_READER_H
