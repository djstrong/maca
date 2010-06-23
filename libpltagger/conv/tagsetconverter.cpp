#include <libpltagger/conv/fold.h>
#include <libpltagger/conv/tagsetconverter.h>
#include <libpltagger/conv/layer.h>
#include <libpltagger/conv/joinlayer.h>
#include <libpltagger/conv/splitlayer.h>
#include <libpltagger/conv/tagrulelayer.h>
#include <libpltagger/conv/tagconvert.h>
#include <libtoki/foreach.h>

namespace PlTagger { namespace Conversion {

	TagsetConverter::TagsetConverter()
		: layers_()
	{
	}

	TagsetConverter::TagsetConverter(const Toki::Config::Node& cfg)
		: layers_()
	{
		foreach (const Toki::Config::Node::value_type &v, cfg) {
			if (v.first == "tag_rule") {
				TagRuleLayer* trl;
				if (!layers_.empty()
						&& (trl = dynamic_cast<TagRuleLayer*>(layers_.back()))) {
					trl->append_rule(v.second);
				}
				layers_.push_back(new TagRuleLayer(v.second));
			} else if (v.first == "convert") {
				layers_.push_back(new TagConvertLayer(v.second));
			} else if (v.first == "join_rule") {
				JoinLayer* jl;
				if (!layers_.empty()
						&& (jl = dynamic_cast<JoinLayer*>(layers_.back()))) {
					jl->append_rule(v.second);
				}
				layers_.push_back(new JoinLayer(v.second));
			} else if (v.first == "split") {
				layers_.push_back(new TwoSplitLayer(v.second));
			}
		}
		if (layers_.empty()) throw 9;
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
				throw PlTaggerError("Conversion layer tagset mismatch");
			}
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
		assert(layers_.back()->source() == NULL || layers_.back()->get_next_token() == NULL);
		layers_.front()->set_source(src);
		Token* t;
		while ((t = layers_.back()->get_next_token())) {
			sink(t);
		}
	}

	void TagsetConverter::convert_simple(const std::vector<Token *>& v, boost::function<void(Token *)>sink)
	{
		convert_container(v, sink);
	}

	void TagsetConverter::convert_ambiguous(const std::vector<std::vector<Token *> >& v, boost::function<void(Token *)>sink)
	{
		size_t min_len_path;
		//size_t min_len =
		find_shortest(v, min_len_path);
		convert_simple(v[min_len_path], sink);
	}

} /* end ns Conversion */ } /* end ns PlTagger */
