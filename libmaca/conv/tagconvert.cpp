#include <libmaca/conv/tagconvert.h>
#include <libcorpus2/token.h>
#include <libcorpus2/tagsetmanager.h>
#include <libpwrutils/foreach.h>

namespace Maca {
namespace Conversion {

TagConverter::TagConverter(const Corpus2::Tagset& from, const Corpus2::Tagset& to)
	: tagset_from_(from), tagset_to_(to), late_check_(true)
{
	from.pos_dictionary().create_mapping_to(
			to.pos_dictionary(), pos_mapping_);
	from.attribute_dictionary().create_mapping_to(
			to.attribute_dictionary(), attribute_mapping_);
	from.value_dictionary().create_mapping_to(
			to.value_dictionary(), value_mapping_);
}

Corpus2::Tag TagConverter::cast(const Corpus2::Tag& from) const
{
	pos_map_t::const_iterator pi = pos_mapping_.find(from.pos_id());
	assert(pi != pos_mapping_.end());
	Corpus2::Tag to(tagset_to_.id(), pi->second);
	to.values().resize(tagset_to_.attribute_dictionary().size());
	memset(&to.values()[0], 0, to.values().size());

	foreach (Corpus2::value_idx_t v, from.values()) {
		if (v > 0) {
			std::map<Corpus2::value_idx_t, Corpus2::value_idx_t>::const_iterator i;
			i = value_mapping_.find(v);
			if (i != value_mapping_.end()) {
				Corpus2::attribute_idx_t a = tagset_to_.get_value_attribute(
					i->second);
				to.values()[a] = i->second;
			} else {
				if (late_check_) {
					std::cerr << "TagConverter: Value not found in map: "
						<< tagset_from().value_dictionary().get_string(v)
						<< "\n";
				}
			}
		}
	}
	return to;
}

void TagConverter::add_override(const std::string& from,
		const std::string& to)
{
	Corpus2::pos_idx_t pos = tagset_from_.pos_dictionary().get_id(from);
	if (tagset_from_.pos_dictionary().is_id_valid(pos)) {
		Corpus2::pos_idx_t pos2 = tagset_to_.pos_dictionary().get_id(to);
		if (tagset_to_.pos_dictionary().is_id_valid(pos2)) {
			pos_mapping_[pos] = pos2;
		} else {
			std::cerr << "Invalid POS/POS mapping override, "
				<< to << " not a valid POS in "
				<< tagset_to().id_string() << "\n";
		}
	} else {
		Corpus2::value_idx_t vto = tagset_to_.value_dictionary().get_id(to);
		if (tagset_to_.value_dictionary().is_id_valid(vto)) {
			Corpus2::attribute_idx_t a;
			a = tagset_from_.attribute_dictionary().get_id(from);
			if (tagset_from_.attribute_dictionary().is_id_valid(a)) {
				foreach (Corpus2::value_idx_t vfrom,
						tagset_from_.get_attribute_values(a)) {
					value_mapping_[vfrom] = vto;
				}
			} else {
				Corpus2::value_idx_t vfrom;
				vfrom = tagset_from_.value_dictionary().get_id(from);
				if (tagset_to_.value_dictionary().is_id_valid(vfrom)) {
					value_mapping_[vfrom] = vto;
				} else {
					std::cerr << "Invalid POS/POS mapping override, "
						<< from << " not a valid POS in "
						<< tagset_from().id_string() << "\n";
				}
			}
		} else {
			std::cerr << "Invalid mapping override, "
				<< to << " not a valid value in "
				<< tagset_to().id_string() << "\n";
		}
	}
}

bool TagConverter::is_complete(std::ostream* os, bool all /*=false*/) const
{
	bool rv = true;
	for (Corpus2::pos_idx_t p = static_cast<Corpus2::pos_idx_t>(0);
			p < tagset_from().pos_dictionary().size(); ++p) {
		pos_map_t::const_iterator pi = pos_mapping_.find(p);
		if (pi == pos_mapping_.end()) {
			if (os) (*os) << "No mapping for POS "
				<< tagset_from().pos_dictionary().get_string(p)
				<< " (" << (int)p << ")";
			rv = false;
			if (!all) return rv;
			if (os) (*os) << "\n";
		}
		if (!tagset_to().pos_dictionary().is_id_valid(pi->second)) {
			if (os) (*os) << "Mapping for POS "
				<< tagset_from().pos_dictionary().get_string(p) << " ("
				<< (int)p << ") is invalid (" << (int)pi->second << ")";
			rv = false;
			if (!all) return rv;
			if (os) (*os) << "\n";
		}
	}
	for (Corpus2::attribute_idx_t p = static_cast<Corpus2::attribute_idx_t>(0);
			p < tagset_from().attribute_dictionary().size(); ++p) {
		attribute_map_t::const_iterator pi = attribute_mapping_.find(p);
		if (pi != attribute_mapping_.end()) {
			Corpus2::attribute_idx_t to = pi->second;
			if (!tagset_to().attribute_dictionary().is_id_valid(to)) {
				if (os) (*os) << "Mapping for attribute "
					<< tagset_from().attribute_dictionary().get_string(p)
					<< " (" << (int)p << ")" << " is invalid"
					<< " (" << (int)to << ")";
				rv = false;
				if (!all) return rv;
				if (os) (*os) << "\n";
			}
		}
	}
	for (Corpus2::value_idx_t p = static_cast<Corpus2::value_idx_t>(0);
			p < tagset_from().value_dictionary().size(); ++p) {
		value_map_t::const_iterator pi = value_mapping_.find(p);
		if (pi == value_mapping_.end()) {
			if (os) (*os) << "No mapping for value "
				<< tagset_from().value_dictionary().get_string(p)
				<< " (" << (int)p << ")";
			rv = false;
			if (!all) return rv;
			if (os) (*os) << "\n";
		}
		if (!tagset_to().value_dictionary().is_id_valid(pi->second)) {
			if (os) (*os) << "Mapping for value "
				<< tagset_from().value_dictionary().get_string(p) << " ("
				<< (int)p << ") is invalid (" << (int)pi->second << ")";
			rv = false;
			if (!all) return rv;
			if (os) (*os) << "\n";
		}
	}
	return rv;
}

TagConvertLayer::TagConvertLayer(const TagConverter &tc)
	: tc_(tc)
{
}

TagConvertLayer* TagConvertLayer::clone() const
{
	return new TagConvertLayer(*this);
}

TagConvertLayer::TagConvertLayer(const Config::Node& cfg)
	: tc_(Corpus2::get_named_tagset(cfg.get<std::string>("tagset_from")),
		Corpus2::get_named_tagset(cfg.get<std::string>("tagset_to")))
{
	foreach (const Config::Node::value_type &v, cfg) {
		if (v.first == "override") {
			std::string o = v.second.data();
			size_t colon = o.find(':');
			if (colon != std::string::npos) {
				std::string o_from = o.substr(0, colon);
				std::string o_to = o.substr(colon + 1);
				tc_.add_override(o_from, o_to);
			}
		}
	}
	bool check_all = cfg.get("check-all", true);
	std::string check = cfg.get("check", "warn");
	if (check == "warn") {
		tc_.is_complete(&std::cerr, check_all);
	} else if (check == "err") {
		if (!tc_.is_complete(&std::cerr, check_all)) {
			throw MacaError("TagConverter not complete and check=err");
		}
	} else if (check != "ignore") {
		throw MacaError("TagConverter check neither warn, err nor ignore");
	}
	tc_.set_late_check(cfg.get("late-check", true));
}

Corpus2::Token* TagConvertLayer::get_next_token()
{
	Corpus2::Token* t = source()->get_next_token();
	if (t != NULL) {
		foreach (Corpus2::Lexeme& lex, t->lexemes()) {
			if (lex.tag().tagset_id() != tagset_from().id()) {
				std::cerr << lex.tag().tagset_id()
					<< " vs " << tagset_from().id();
				assert(lex.tag().tagset_id() == tagset_from().id());
			}
			lex.set_tag(tc_.cast(lex.tag()));
			assert(lex.tag().tagset_id() == tagset_to().id());
		}
		t->remove_duplicate_lexemes();
	}
	return t;
}

const Corpus2::Tagset& TagConvertLayer::tagset_from() const
{
	return tc_.tagset_from();
}

const Corpus2::Tagset& TagConvertLayer::tagset_to() const
{
	return tc_.tagset_to();
}

} /* end ns Conversion */
} /* end ns Maca */
