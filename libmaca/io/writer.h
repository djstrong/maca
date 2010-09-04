#ifndef LIBMACA_IO_WRITER_H
#define LIBMACA_IO_WRITER_H

#include <libmaca/chunk.h>

#include <loki/Factory.h>
#include <loki/Singleton.h>

namespace Maca {

	/**
	 * Base class for writers of Maca tokens
	 */
	class TokenWriter
	{
	public:
		/**
		 * Constructor. A TokenWriter operates within a tagset, outputs data
		 * to a stream and may have some options passed.
		 */
		TokenWriter(std::ostream& os, const Tagset& tagset, const string_range_vector& params);

		/// Destructor
		virtual ~TokenWriter();

		virtual void write_token(const Token& t) = 0;

		/// Convenience wrapper for a write-and-delete action
		void write_token_dispose(Token* t);

		virtual void write_sentence(const Sentence& s) = 0;

		virtual void write_chunk(const Chunk& p) = 0;

		std::ostream& os() {
			return os_;
		}

		const Tagset& tagset() {
			return tagset_;
		}

		void finish();

		/**
		 * Factory interface for creating writers from string identifiers
		 *
		 * Mostly a convenience function to avoid having client code refer directly
		 * to the factory instance.
		 *
		 * @param class_id the unique class identifier
		 * @param os the output stream to pass to the writer's constructor
		 * @param tagset the tagset to pass to the writer's constructor
		 */
		static TokenWriter* create(const std::string class_id,
				std::ostream& os,
				const Tagset& tagset,
				const string_range_vector& params);

		/**
		 * Factory interface for creating writers from string identifiers.
		 *
		 * Params are split from the class id and then the more general version
		 * is called. Parameters are expected to be comma-separated from the
		 * class id.
		 */
		static TokenWriter* create(const std::string class_id_params,
				std::ostream& os,
				const Tagset& tagset);

		/**
		 * Function to get a vector of available writer type strings.
		 */
		static std::vector<std::string> available_writer_types();

		/**
		 * Function to get a vector of available writer type strings with help
		 * strings appended
		 */
		static std::vector<std::string> available_writer_types_help();

		/**
		 * Convenience template for registering TokenWriter derived classes.
		 */
		template <typename T>
		static bool register_writer(const std::string& class_id, const std::string& help = "");


	protected:
		virtual void do_footer() {}

		void indent_more(int n = 1);

		void indent_less(int n = 1);

		int indent_level() const {
			return indent_;
		}

		std::ostream& osi();

	private:
		std::ostream& os_;

		const Tagset& tagset_;

		bool needs_footer_;

		int indent_;
	};

	typedef Loki::Factory<
		TokenWriter, // The base class for objects created in the factory
		std::string, // Identifier type
		Loki::TL::MakeTypelist< std::ostream&, const Tagset&, const string_range_vector& >::Result
		// TokenLayer constructor arguments' types specification
	> TokenWriterFactoryType;

	struct TokenWriterFactory
	{
		TokenWriterFactoryType factory;
		std::map<std::string, std::string> help;
	};

	/**
	 * Declaration of the TokenWriter factory as a singleton Loki object factory.
	 * The factory instance can be accessed as TokenLayerFactory::Instance().
	 * It is assumed that all derived classes have the same constructor signature.
	 */
	typedef Loki::SingletonHolder<
		TokenWriterFactory,
		Loki::CreateUsingNew, // default, needed to change the item below
		Loki::LongevityLifetime::DieAsSmallObjectChild // Required per libloki docs
	>
	TokenWriterFactorySingleton;

	/**
	 * Convenience typedef for the exception type the factory throws
	 */
	typedef Loki::DefaultFactoryError<
		std::string, TokenWriter
	>::Exception
	TokenWriterFactoryException;

	/**
	 * Convenience template TokenWriter creation function
	 */
	template <typename T>
	inline
	T* writer_creator(std::ostream& os, const Tagset& tagset, const string_range_vector& params)
	{
		return new T(os, tagset, params);
	}

	template <typename T>
	bool TokenWriter::register_writer(const std::string& class_id, const std::string& help)
	{
		bool ret = TokenWriterFactorySingleton::Instance().factory.Register(class_id, writer_creator<T>);
		if (ret) {
			TokenWriterFactorySingleton::Instance().help[class_id] = help;
		}
		return ret;
	}

} /* end ns Maca */

#endif // LIBMACA_IO_WRITER_H
