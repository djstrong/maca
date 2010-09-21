#include <libmaca/conv/fold.h>
#include <libmaca/conv/tagsetconverter.h>
#include <libmaca/conv/layer.h>
#include <libmaca/conv/joinlayer.h>
#include <libmaca/conv/splitlayer.h>
#include <libmaca/conv/tagrulelayer.h>
#include <libmaca/conv/tagconvert.h>
#include <libpwrutils/foreach.h>

namespace Maca { namespace Conversion {

TagsetConverter::TagsetConverter()
	: layers_()
{
}

TagsetConverter::TagsetConverter(const Config::Node& cfg)
	: layers_()
{
	foreach (Config::Node::value_type v, cfg) {
		std::string tagset_string = v.second.get("tagset", "");
		if (tagset_string.empty() && !layers_.empty()) {
			v.second.put("tagset", layers_.back()->tagset_to().name());
		}
		if (v.first == "tag_rule" || v.first == "tag") {
			if (!layers_.empty()) {
				Layer* layer = layers_.back();
				TagRuleLayer* tag, *regex;
				tag = dynamic_cast<TagRuleLayer*>(layer);
				regex = dynamic_cast<RegexTagRuleLayer*>(layer);
				bool separate = v.second.get("separate", false);
				if (!separate && tag != NULL && regex == NULL) {
					tag->append_rule(v.second);
				} else {
					add_layer(new TagRuleLayer(v.second));
				}
			} else {
				add_layer(new TagRuleLayer(v.second));
			}
		} else if (v.first == "regex_tag_rule" || v.first == "re_tag") {
			add_layer(new RegexTagRuleLayer(v.second));
		} else if (v.first == "convert") {
			add_layer(new TagConvertLayer(v.second));
		} else if (v.first == "join_rule" || v.first == "join") {
			if (!layers_.empty()) {
				JoinLayer* jl = dynamic_cast<JoinLayer*>(layers_.back());
				bool separate = v.second.get("separate", false);
				if (!separate && jl != NULL) {
					jl->append_rule(v.second);
				} else {
					add_layer(new JoinLayer(v.second));
				}
			} else {
				add_layer(new JoinLayer(v.second));
			}
		} else if (v.first == "split") {
			add_layer(new TwoSplitLayer(v.second));
		} else if (v.first == "3split") {
			add_layer(new ThreeSplitLayer(v.second));
		} else {
			std::cerr << "Unknown conversion layer type: "
				<< v.first << "\n";
		}
	}
	if (layers_.empty()) throw MacaError("Empty tagset converter");
}

TagsetConverter::~TagsetConverter()
{
	foreach (Layer* l, layers_) {
		delete l;
	}
}

TagsetConverter* TagsetConverter::clone() const
{
	TagsetConverter* copy = new TagsetConverter;
	foreach (Layer* l, layers_) {
		copy->add_layer(l->clone());
	}
	return copy;
}

void TagsetConverter::add_layer(Layer* l)
{
	if (!layers_.empty()) {
		if (l->tagset_from().id() != layers_.back()->tagset_to().id()) {
			throw Corpus2::TagsetMismatch("TagsetConverter::add_layer",
				layers_.back()->tagset_to().id(),
				l->tagset_from().id());
		}
		l->set_source(layers_.back());
	}
	layers_.push_back(l);
}

const Corpus2::Tagset& TagsetConverter::tagset_from() const
{
	assert(!layers_.empty());
	return layers_.front()->tagset_from();
}

const Corpus2::Tagset& TagsetConverter::tagset_to() const
{
	assert(!layers_.empty());
	return layers_.back()->tagset_to();
}

void TagsetConverter::convert(Corpus2::TokenSource* src,
		boost::function<void (Corpus2::Token*)> sink)
{
	assert(!layers_.empty());
	assert((layers_.front()->source() == NULL) ||
			(layers_.back()->get_next_token() == NULL));
	layers_.front()->set_source(src);
	while (Corpus2::Token* t = layers_.back()->get_next_token()) {
		assert(t->lexemes()[0].tag().tagset_id() ==
				layers_.back()->tagset_to().id());
		sink(t);
	}
	layers_.front()->set_source(NULL);
}

void TagsetConverter::convert_simple(const std::vector<Corpus2::Token *>& v,
		boost::function<void(Corpus2::Token *)>sink)
{
	convert_container(v, sink);
}

void TagsetConverter::convert_ambiguous(
		const std::vector<std::vector<Corpus2::Token *> >& v,
		boost::function<void(Corpus2::Token *)>sink, bool warn_on_failure /*=true*/)
{
	std::vector< std::vector<Corpus2::Token *> > conv_v;
	foreach (const std::vector<Corpus2::Token*>& path, v) {
		conv_v.push_back(std::vector<Corpus2::Token*>());
		boost::function<void (Corpus2::Token*)> sink = boost::bind(
				&std::vector<Corpus2::Token*>::push_back, boost::ref(conv_v.back()),
				_1);
		convert_container(path, sink);
	}
	if (!try_fold_paths(conv_v, sink)) {
		if (warn_on_failure) {
			std::cerr << "!!! Path folding failed,"
				<< " returning shortest from: ";
			foreach (const std::vector<Corpus2::Token*>& path, v) {
				std::cerr << " >> ";
				foreach (Corpus2::Token* t, path) {
					std::cerr << t->orth_utf8() << " ";
				}
			}
			std::cerr << "\n";
		}
		choose_shortest_path(conv_v, sink);
	}
}

Corpus2::Sentence* TagsetConverter::convert_sentence(Corpus2::Sentence* s)
{
	Corpus2::Sentence* res = new Corpus2::Sentence;
	boost::function<void (Corpus2::Token*)> adder = boost::bind(&Corpus2::Sentence::append,
			res, _1);
	std::vector<Corpus2::Token*>::iterator i = s->tokens().begin();
	if (i != s->tokens().end()) {
		std::vector<Corpus2::Token*>::iterator b = i;
		++i;
		while (i != s->tokens().end()) {
			const Corpus2::Token& t = **i;
			if (t.wa() != PwrNlp::Whitespace::None) {
				convert_container(
						boost::sub_range< std::vector<Corpus2::Token*> >(b, i),
						adder);
				b = i;
			}
			++i;
		}
		convert_container(
				boost::sub_range< std::vector<Corpus2::Token*> >(b, i),
				adder);
	}
	s->tokens().clear();
	delete s;
	return res;
}

} /* end ns Conversion */ } /* end ns Maca */
