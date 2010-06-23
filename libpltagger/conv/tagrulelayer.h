#ifndef LIBPLTAGGER_CONV_TAGRULELAYER_H
#define LIBPLTAGGER_CONV_TAGRULELAYER_H

#include <libpltagger/conv/layer.h>
#include <libpltagger/conv/tagrule.h>
#include <libtoki/confignode.h>

namespace PlTagger { namespace Conversion {

	class TagRuleLayer : public OneTagsetLayer
	{
	public:
		TagRuleLayer(const Tagset& tagset);

		TagRuleLayer(const Toki::Config::Node& cfg);

		void append_rule(const TagRule& tr);

		void append_rule(const Toki::Config::Node& cfg);

		Token* get_next_token();

	private:
		std::vector<TagRule> rules_;
	};

} /* end ns Conversion */ } /* end ns PlTagger */

#endif // LIBPLTAGGER_CONV/TAGRULELAYER_H
