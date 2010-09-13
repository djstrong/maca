#ifndef LIBMACA_UTIL_SENTENCEANALYSER_H
#define LIBMACA_UTIL_SENTENCEANALYSER_H

#include <libtoki/tokenizer/tokenizer.h>
#include <libtoki/sentencesplitter.h>
#include <libmaca/morph/morphanalyser.h>
#include <libmaca/util/confignode.h>

namespace Maca {

	class SentenceAnalyser : public Toki::UnicodeSink
	{
	public:
		SentenceAnalyser(const boost::shared_ptr<Toki::Tokenizer>& tok,
			const boost::shared_ptr<MorphAnalyser>& ma);

		SentenceAnalyser(const Config::Node& cfg);

		SentenceAnalyser(const Config::Node& cfg,
				const Toki::Config::Node& toki_config_override);

		static boost::shared_ptr<SentenceAnalyser> create_from_named_config(
				const std::string& config_name);

		static boost::shared_ptr<SentenceAnalyser> create_from_named_config(
				const std::string& config_name,
				const std::string& toki_config_override);

		Sentence* get_next_sentence();

		typedef boost::function<void (Sentence*)> sentence_sink_t;

		/**
		 * Convenience function to process all sentences that can be gathered
		 * and feed them one by one to the sink function.
		 * @return true if the sink was called at least once, false otherwise
		 *         (meaning there were no sentences)
		 */
		bool process(sentence_sink_t sink);

		const Toki::Tokenizer& tokenizer() const {
			return *tok_;
		}

		const MorphAnalyser& analyser() const {
			return *ma_;
		}

		const Tagset& tagset() const {
			return ma_->tagset();
		}

	protected:
		void new_input_source();

	private:
		boost::shared_ptr<Toki::Tokenizer> tok_;

		Toki::SentenceSplitter sp_;

		boost::shared_ptr<MorphAnalyser> ma_;
	};



} /* end ns Maca */

#endif // LIBMACA_UTIL_SENTENCEANALYSER_H
