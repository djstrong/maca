#include <libmaca/morph/convertinganalyser.h>
#include <libmaca/util/settings.h>
#include <fstream>

namespace Maca {

const char* ConvertingAnalyser::identifier = "wrap_convert";

bool ConvertingAnalyser::registered =
		MorphAnalyser::register_analyser<ConvertingAnalyser>();

ConvertingAnalyser::ConvertingAnalyser(MorphAnalyser *ma,
		Conversion::TagsetConverter *conv)
	: MorphAnalyser(&conv->tagset_to()), wrapped_(ma), converter_(conv)
{
	require_matching_tagsets(converter_->tagset_from(), *wrapped_,
			"Converting analyser creation (from)");
	require_matching_tagsets(converter_->tagset_to(), *this,
			"Converting analyser creation (to)");
}

ConvertingAnalyser::ConvertingAnalyser(const Config::Node &cfg)
	: MorphAnalyser(cfg), wrapped_(NULL), converter_(NULL)
{
	std::string wrapped_id = cfg.get("wrapped_class", "");
	Config::Node cfg2 = cfg;
	cfg2.put("tagset", cfg.get("wrapped_tagset", ""));
	try {
		wrapped_.reset(MorphAnalyser::create(wrapped_id, cfg2));
	} catch (MorphAnalyserFactoryException&) {
		if (cfg.get("plugin_autoload", true)) {
			if (MorphAnalyser::load_plugin(wrapped_id, false)) {
				try {
					wrapped_.reset(MorphAnalyser::create(wrapped_id, cfg2));
				} catch (MorphAnalyserFactoryException&) {
					throw MacaError("Unknown analyser type: " + wrapped_id +
							" (plugin found but create failed)");
				}
			} else {
				throw MacaError("Unknown analyser type: " + wrapped_id +
						" (plugin not found)");
			}
		} else {
			throw MacaError("Unknown analyser type: " + wrapped_id);
		}
	}

	std::string fn = cfg.get("wrapped_converter", "");
	std::ifstream ifs;
	Path::Instance().open_stream_or_throw(fn, ifs, "converter");
	Config::Node conv_cfg = Config::from_stream(ifs);
	converter_.reset(new Conversion::TagsetConverter(conv_cfg));

	require_matching_tagsets(converter_->tagset_from(), *wrapped_,
			"Converting analyser creation (from)");
	require_matching_tagsets(converter_->tagset_to(), *this,
			"Converting analyser creation (to)");
}

ConvertingAnalyser* ConvertingAnalyser::clone() const
{
	std::auto_ptr<ConvertingAnalyser> copy;
	copy.reset(new ConvertingAnalyser(wrapped_->clone(), converter_->clone()));
	return copy.release();
}

bool ConvertingAnalyser::process_functional(const Toki::Token &t,
		boost::function<void (Corpus2::Token *)> sink)
{
	std::vector<Corpus2::Token*> tv = wrapped_->process(t);
	converter_->convert_simple(tv, sink);
	return true;
}



} /* end ns Maca */
