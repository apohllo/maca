#ifndef LIBPLTAGGER_LEXEME_H
#define LIBPLTAGGER_LEXEME_H

#include <unicode/unistr.h>

#include <libpltagger/tag.h>
#include <libtoki/util.h>

namespace PlTagger {

	class Lexeme
		: boost::equality_comparable<Lexeme>, boost::less_than_comparable<Lexeme>
	{
	public:
		Lexeme();

		Lexeme(const UnicodeString& lemma, const Tag& tag);

		static Lexeme create(const UnicodeString& lemma, const Tag& tag);

		const UnicodeString& lemma() const {
			return lemma_;
		}

		void set_lemma(const UnicodeString& l) {
			lemma_ = l;
		}

		const std::string lemma_utf8() const {
			return Toki::Util::to_utf8(lemma_);
		}

		const Tag& tag() const {
			return tag_;
		}

		void set_tag(const Tag& t) {
			tag_ = t;
		}

		bool operator<(const Lexeme& other) const;

		bool operator==(const Lexeme& other) const;

	private:
		UnicodeString lemma_;

		Tag tag_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_LEXEME_H