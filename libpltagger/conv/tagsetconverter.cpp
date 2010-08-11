#include <libpltagger/conv/fold.h>
#include <libpltagger/conv/tagsetconverter.h>
#include <libpltagger/conv/layer.h>
#include <libpltagger/conv/joinlayer.h>
#include <libpltagger/conv/splitlayer.h>
#include <libpltagger/conv/tagrulelayer.h>
#include <libpltagger/conv/tagconvert.h>
#include <libtoki/util/foreach.h>

namespace PlTagger { namespace Conversion {

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
				TagRuleLayer* trl;
				if (!layers_.empty()
						&& (trl = dynamic_cast<TagRuleLayer*>(layers_.back()))) {
					trl->append_rule(v.second);
				} else {
					add_layer(new TagRuleLayer(v.second));
				}
			} else if (v.first == "convert") {
				add_layer(new TagConvertLayer(v.second));
			} else if (v.first == "join_rule" || v.first == "join") {
				JoinLayer* jl;
				if (!layers_.empty()
						&& (jl = dynamic_cast<JoinLayer*>(layers_.back()))) {
					jl->append_rule(v.second);
				} else {
					add_layer(new JoinLayer(v.second));
				}
			} else if (v.first == "split") {
				add_layer(new TwoSplitLayer(v.second));
			} else if (v.first == "3split") {
				add_layer(new ThreeSplitLayer(v.second));
			}
		}
		if (layers_.empty()) throw PlTaggerError("Empty tagset converter");
	}

	TagsetConverter::~TagsetConverter()
	{
		foreach (Layer* l, layers_) {
			delete l;
		}
	}

	void TagsetConverter::add_layer(Layer* l)
	{
		if (!layers_.empty()) {
			if (l->tagset_from().id() != layers_.back()->tagset_to().id()) {
				throw TagsetMismatch("TagsetConverter::add_layer",
					layers_.back()->tagset_to().id(),
					l->tagset_from().id());
			}
			l->set_source(layers_.back());
		}
		layers_.push_back(l);
	}

	const Tagset& TagsetConverter::tagset_from() const
	{
		assert(!layers_.empty());
		return layers_.front()->tagset_from();
	}

	const Tagset& TagsetConverter::tagset_to() const
	{
		assert(!layers_.empty());
		return layers_.back()->tagset_to();
	}

	void TagsetConverter::convert(TokenSource* src, boost::function<void (Token*)> sink)
	{
		assert(!layers_.empty());
		assert((layers_.front()->source() == NULL) || (layers_.back()->get_next_token() == NULL));
		layers_.front()->set_source(src);
		Token* t;
		while ((t = layers_.back()->get_next_token())) {
			assert(t->lexemes()[0].tag().tagset_id() == layers_.back()->tagset_to().id());
			sink(t);
		}
		layers_.front()->set_source(NULL);
	}

	void TagsetConverter::convert_simple(const std::vector<Token *>& v, boost::function<void(Token *)>sink)
	{
		convert_container(v, sink);
	}

	void TagsetConverter::convert_ambiguous(const std::vector<std::vector<Token *> >& v, boost::function<void(Token *)>sink)
	{
		std::vector< std::vector<Token *> > conv_v;
		foreach (const std::vector<Token*>& path, v) {
			conv_v.push_back(std::vector<Token*>());
			boost::function<void (Token*)> sink;
			sink = boost::bind(&std::vector<Token*>::push_back, boost::ref(conv_v.back()), _1);
			convert_container(path, sink);
		}
		if (!try_fold_paths(conv_v, sink)) {
			std::cerr << "!!! Path folding failed, returning shortest from: ";
			foreach (const std::vector<Token*>& path, v) {
				std::cerr << " >> ";
				foreach (Token* t, path) {
					std::cerr << t->orth_utf8() << " ";
				}
			}
			std::cerr << "\n";
			choose_shortest_path(conv_v, sink);
		}
	}

	Sentence* TagsetConverter::convert_sentence(Sentence* s)
	{
		Sentence* res = new Sentence;
		boost::sub_range< std::vector<Token*> > tsr;
		boost::function<void (Token*)> adder = boost::bind(&Sentence::append, res, _1);
		std::vector<Token*>::iterator i = s->tokens().begin();
		if (i != s->tokens().end()) {
			std::vector<Token*>::iterator b = i;
			tsr.begin() = i;
			++i;
			while (i != s->tokens().end()) {
				const Token& t = **i;
				if (t.wa() != Toki::Whitespace::None) {
					convert_container(boost::sub_range< std::vector<Token*> > (b, i), adder);
					b = i;
				}
				++i;
			}
			convert_container(boost::sub_range< std::vector<Token*> >(b, i), adder);
		}
		s->tokens().clear();
		delete s;
		return res;
	}

} /* end ns Conversion */ } /* end ns PlTagger */
