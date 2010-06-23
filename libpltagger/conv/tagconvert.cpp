#include <libpltagger/conv/tagconvert.h>
#include <libpltagger/token.h>
#include <libpltagger/tagsetmanager.h>
#include <libtoki/foreach.h>

namespace PlTagger { namespace Conversion {

	TagConverter::TagConverter(const Tagset& from, const Tagset& to)
		: tagset_from_(from), tagset_to_(to)
	{
		from.pos_dictionary().create_mapping_to(to.pos_dictionary(), pos_mapping_);
		from.attribute_dictionary().create_mapping_to(to.attribute_dictionary(), attribute_mapping_);
		from.value_dictionary().create_mapping_to(to.value_dictionary(), value_mapping_);
	}

	Tag TagConverter::cast(const Tag& from) const
	{
		pos_map_t::const_iterator pi = pos_mapping_.find(from.pos_id());
		assert(pi != pos_mapping_.end());
		Tag to(tagset_to_.id(), pi->second);
		to.values().resize(tagset_to_.attribute_dictionary().size());
		memset(&to.values()[0], 0, to.values().size());

		foreach (value_idx_t v, from.values()) {
			if (v > 0) {
				std::map<value_idx_t, value_idx_t>::const_iterator i;
				i = value_mapping_.find(v);
				if (i != value_mapping_.end()) {
					attribute_idx_t a = tagset_to_.get_value_attribute(i->second);
					to.values()[a] = i->second;
				}
			}
		}
		return to;
	}

	void TagConverter::add_override(const std::string& from, const std::string& to)
	{
		pos_idx_t pos = tagset_from_.pos_dictionary().get_id(from);
		if (tagset_from_.pos_dictionary().is_id_valid(pos)) {
			pos_idx_t pos2 = tagset_to_.pos_dictionary().get_id(to);
			if (tagset_to_.pos_dictionary().is_id_valid(pos2)) {
				pos_mapping_[pos] = pos2;
			}
		} else {
			value_idx_t v = tagset_to_.value_dictionary().get_id(to);
			if (tagset_to_.value_dictionary().is_id_valid(v)) {
				attribute_idx_t a = tagset_from_.attribute_dictionary().get_id(from);
				if (tagset_from_.attribute_dictionary().is_id_valid(a)) {
					foreach (value_idx_t vfrom, tagset_from_.get_attribute_values(a)) {
						value_mapping_[vfrom] = v;
					}
				} else {
					value_idx_t v2 = tagset_from_.value_dictionary().get_id(to);
					if (tagset_to_.value_dictionary().is_id_valid(v)) {
						value_mapping_[v2] = v;
					}
				}
			}
		}
	}

	TagConvertLayer::TagConvertLayer(const TagConverter &tc)
		: tc_(tc)
	{
	}

	TagConvertLayer::TagConvertLayer(const Toki::Config::Node& cfg)
		: tc_(get_named_tagset(cfg.get<std::string>("tagset_from")),
			get_named_tagset(cfg.get<std::string>("tagset_to")))
	{
		foreach (const Toki::Config::Node::value_type &v, cfg) {
			if (v.first == "override") {
				std::string o = v.second.data();
				size_t colon = o.find(':');
				if (colon != std::string::npos) {
					std::string o_from = o.substr(0, colon - 1);
					std::string o_to = o.substr(colon);
					tc_.add_override(o_from, o_to);
				}
			}
		}
	}

	Token* TagConvertLayer::get_next_token()
	{
		Token* t = source()->get_next_token();
		if (t != NULL) {
			foreach (Lexeme& lex, t->lexemes()) {
				lex.tag() = (tc_.cast(lex.tag()));
			}
		}
		return t;
	}

	const Tagset& TagConvertLayer::tagset_from() const
	{
		return tc_.tagset_from();
	}

	const Tagset& TagConvertLayer::tagset_to() const
	{
		return tc_.tagset_to();
	}

} /* end ns Conversion */ } /* end ns PlTagger */
