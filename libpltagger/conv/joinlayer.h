#ifndef LIBPLTAGGER_CONV_JOINLAYER_H
#define LIBPLTAGGER_CONV_JOINLAYER_H

#include <libpltagger/conv/layer.h>
#include <libpltagger/conv/joinrule.h>
#include <libtoki/confignode.h>

namespace PlTagger { namespace Conversion {

	class JoinLayer : public OneTagsetLayer
	{
	public:
		JoinLayer(const Tagset& tagset);

		JoinLayer(const Toki::Config::Node& cfg);

		void append_rule(const JoinRule& rule);

		void append_rule(const Toki::Config::Node& cfg);

		Token* get_next_token();

	private:
		std::vector<JoinRule> rules_;

		Token* buf_;
	};

} /* end ns Conversion */ } /* end ns PlTagger */

#endif // LIBPLTAGGER_CONV_JOINLAYER_H
