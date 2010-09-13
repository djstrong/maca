#ifndef LIBMACA_MORPHANALYSER_H
#define LIBMACA_MORPHANALYSER_H

#include <libtoki/token.h>
#include <libmaca/util/confignode.h>
#include <libmaca/sentence.h>
#include <libmaca/token.h>
#include <libmaca/tagset.h>

#include <boost/function.hpp>

#include <loki/Factory.h>
#include <loki/Singleton.h>

namespace Maca {

	/**
	 * Base class for morphological analysers.
	 *
	 * A MorphAnalyser analyser takes a Toki token and returns one or more
	 * Maca tokens, each of which contains one or more lexemes. An analyser
	 * returns tags from a specified tagset.
	 */
	class MorphAnalyser
	{
	public:
		/// Constructor for a MorphAnalyser working within a given tagset
		explicit MorphAnalyser(const Tagset* tagset);

		/**
		 * Config node constructor. Recognized keys are:
		 * - tagset - the tagset name to load and use
		 */
		explicit MorphAnalyser(const Config::Node& cfg);

		/// Cloning
		virtual MorphAnalyser* clone() const = 0;

		/// Destructor
		virtual ~MorphAnalyser();

		/// Convenience process_functional wrapper to process a Toki token
		/// and return a vector of Maca tokens.
		std::vector<Token*> process(const Toki::Token& t);

		/// Convenience process_functional wrapper to process a Toki sentence
		/// and return a Maca sentence of analysed tokens
		Sentence* process(const Toki::Sentence& s);

		/// Convenience process_functional wrapper  to process a Toki token
		/// and insert the resulting Maca tokens into the given vector.
		void process(const Toki::Token &t, std::vector<Token*>& vec);

		/// Convenience process_functional wrapper to process a vector of Toki
		/// tokens and return a vector of Maca tokens. The toki tokens are
		/// deleted.
		std::vector<Token*> process_dispose(
				const std::vector<Toki::Token*>& t);

		/// Convenience process_functional wrapper to process a vector of Toki
		/// tokens and insert the resulting Maca tokens into the given
		/// vector. The toki tokens are deleted.
		void process_dispose(const std::vector<Toki::Token*>& t,
				std::vector<Token*>& v);

		/// Convenience process_functional wrapper to process a vector of Toki
		/// tokens and return a vector of Maca tokens. The toki tokens are
		/// deleted.
		Sentence* process_dispose(Toki::Sentence* s);

		/// Convenience process_functional wrapper to process a Toki Sentence
		/// tokens and insert the resulting Maca tokens into a Maca
		/// Sentence. The Toki sentence and tokens are deleted.
		void process_dispose(Toki::Sentence* t, Sentence* v);

		/**
		 * The main token analysis function to be implemented in derived
		 * classes. Takes a Toki token and feeds the resulting Maca tokens
		 * into the sink function object.
		 *
		 * The sink function can be called zero or more times, with zero
		 * indicating that the token has not been recognized at all, and more
		 * than one token meaning that the analyser decided to split the token
		 * into smaller parts.
		 *
		 * Should return true if the sink was called at least once, and false
		 * of no tokens were output.
		 */
		virtual bool process_functional(const Toki::Token &t,
				boost::function<void (Token*)> sink) = 0;

		/**
		 * Convenience function to call process_functional and then dispose of
		 * the incoming Toki tokens.
		 */
		bool process_functional_dispose(const Toki::Token *t,
				boost::function<void (Token*)> sink) {
			bool rv = process_functional(*t, sink);
			delete t;
			return rv;
		}

		/// Tagset accesor
		const Tagset& tagset() const {
			return *tagset_;
		}

		/// Tagset setter
		void set_tagset(const Tagset* tagset) {
			tagset_ = tagset;
		}

		/**
		 * Factory interface for creating analysers from string identifiers
		 *
		 * Mostly a convenience function to avoid having client code refer
		 * directly to the TokenLayerFactory instance.
		 *
		 * @param class_id the unique class identifier
		 * @param input the input source to pass to the layer's constructor
		 * @param props the configuration to pass to the layer's constructor
		 */
		static MorphAnalyser* create(const std::string class_id,
				const Config::Node& props);

		/**
		 * Function to get a vector of available analyser type strings.
		 */
		static std::vector<std::string> available_analyser_types();

		/**
		 * Convert a plugin name to a shared library name that is expected to
		 * contain the plugin.
		 */
		static std::string get_plugin_soname(const std::string& name);

		/**
		 * Load a plugin analyser module
		 */
		static bool load_plugin(const std::string& name, bool quiet = false);

		/**
		 * Convenience template for registering MorphAnalyser derived classes.
		 */
		template <typename T>
		static bool register_analyser(const std::string& class_id);

		/**
		 * Convenience template for registering MorphAnalyser derived classes.
		 * Assumes the identifier is at T::identifier
		 */
		template <typename T>
		static bool register_analyser();

	private:
		/// The tagset used by this analyser
		const Tagset* tagset_;
	};

	/**
	 * Declaration of the MorphAnalyser factory as a singleton Loki object
	 * factory. The factory instance can be accessed as
	 * MorphAnalyserFactory::Instance(). It is assumed that all derived classes
	 * have the same constructor signature.
	 */
	typedef Loki::SingletonHolder<
		Loki::Factory<
			MorphAnalyser, // The base class for objects created in the factory
			std::string, // Identifier type
			Loki::TL::MakeTypelist< const Config::Node& >::Result
			// TokenLayer constructor arguments' types specification
		>,
		Loki::CreateUsingNew, // default, needed to change the item below
		Loki::LongevityLifetime::DieAsSmallObjectChild // per libloki docs
	>
	MorphAnalyserFactory;

	/**
	 * Convenience typedef for the exception type the factory throws
	 */
	typedef Loki::DefaultFactoryError<
		std::string, MorphAnalyser
	>::Exception
	MorphAnalyserFactoryException;



	/**
	 * Convenience template MorphAnalyser creation function
	 */
	template <typename T>
	T* analyser_creator(const Config::Node& props)
	{
		return new T(props);
	}

	template <typename T>
	bool MorphAnalyser::register_analyser(const std::string& class_id)
	{
		return MorphAnalyserFactory::Instance().Register(
				class_id, analyser_creator<T>);
	}

	template <typename T>
	bool MorphAnalyser::register_analyser()
	{
		return MorphAnalyserFactory::Instance().Register(
				T::identifier, analyser_creator<T>);
	}

} /* end ns Maca */

#endif // LIBMACA_MORPHANALYSER_H
