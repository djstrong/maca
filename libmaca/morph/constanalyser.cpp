#include <libmaca/morph/constanalyser.h>

namespace Maca {

	const char* ConstAnalyser::identifier = "const";

	bool ConstAnalyser::registered = MorphAnalyser::register_analyser<ConstAnalyser>();

	ConstAnalyser::ConstAnalyser(const Tagset *tagset, const std::string &tag)
		: MorphAnalyser(tagset), tag_(tagset->parse_simple_tag(tag, false))
	{
	}

	ConstAnalyser::ConstAnalyser(const Tagset *tagset, const Tag &tag)
		: MorphAnalyser(tagset), tag_(tag)
	{
	}

	ConstAnalyser::ConstAnalyser(const Config::Node& cfg)
		: MorphAnalyser(cfg), tag_(), lower_lemma_(false)
	{
		std::string tag_string = cfg.get("tag", "");
		if (tag_string.empty()) throw ConfigValueMissing("tag", "ConstAnalyser");
		tag_ = tagset().parse_simple_tag(tag_string, false);
		lower_lemma_ = cfg.get("lower_lemma", false);
	}

	bool ConstAnalyser::process_functional(const Toki::Token &t, boost::function<void (Token*)> sink)
	{
		Token* tt = new Token(t);
		UnicodeString lemma = t.orth();
		if (lower_lemma_) {
			lemma.toLower();
		};
		tt->add_lexeme(Lexeme(lemma, tag_));
		sink(tt);
		return true;
	}

} /* end ns Maca */
