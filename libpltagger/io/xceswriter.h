#ifndef LIBPLTAGGER_IO_XCESWRITER_H
#define LIBPLTAGGER_IO_XCESWRITER_H

#include <libpltagger/io/writer.h>

namespace PlTagger {

	class XcesWriter : public TokenWriter {
	public:
		XcesWriter(std::ostream& os, const Tagset& tagset, bool force_chunk=true);

		void use_indent(bool v) {
			use_indent_ = v;
		}

		void force_chunk(bool v) {
			force_chunk_ = v;
		}

		static XcesWriter* create_flat(std::ostream& os, const Tagset& tagset);
		static XcesWriter* create_nochunk(std::ostream& os, const Tagset& tagset);
		static XcesWriter* create_flat_nochunk(std::ostream& os, const Tagset& tagset);

		~XcesWriter();

		void write_token(const Token &t);

		void write_sentence(const Sentence &s);

		void write_chunk(const Chunk &c);

	protected:
		void do_header();

		void do_footer();

		void paragraph_head();

		void paragraph_head(const Chunk& c);

		int cid_;

		bool use_indent_;

		bool force_chunk_;
	};


} /* end ns PlTagger */

#endif // LIBPLTAGGER_IO_XCESWRITER_H