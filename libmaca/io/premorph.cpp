#include <libmaca/io/premorph.h>
#include <libmaca/io/xcescommon.h>
#include <libtoki/util/foreach.h>
#include <libxml++/parsers/saxparser.h>
#include <libtoki/sentencesplitter.h>
#include <vector>

namespace Maca {

	class PremorphProcessorImpl : public xmlpp::SaxParser
	{
	public:
		PremorphProcessorImpl(std::ostream& os, Toki::Tokenizer& tok, Maca::MorphAnalyser& ma);

	protected:
		void on_start_document();
		void on_start_element(const Glib::ustring & name,
				const AttributeList& attributes);
		void on_end_element(const Glib::ustring & name);
		void on_cdata_block(const Glib::ustring & text);
		void on_characters(const Glib::ustring & text);
		void on_error(const Glib::ustring &text);
		void on_fatal_error(const Glib::ustring &text);
	private:
		std::ostream& os_;
		std::stringstream buf_;
		Toki::Tokenizer& tok_;
		Maca::MorphAnalyser& ma_;
	};

	PremorphProcessor::PremorphProcessor(std::ostream &os, Toki::Tokenizer& tok, Maca::MorphAnalyser& ma)
		: impl_(new PremorphProcessorImpl(os, tok, ma))
	{
	}

	PremorphProcessor::~PremorphProcessor()
	{
	}

	void PremorphProcessor::parse_stream(std::istream &is)
	{
		impl_->parse_stream(is);
	}

	PremorphProcessorImpl::PremorphProcessorImpl(std::ostream& os, Toki::Tokenizer& tok, Maca::MorphAnalyser& ma)
		: xmlpp::SaxParser(), os_(os), tok_(tok), ma_(ma)
	{
	}

	void PremorphProcessorImpl::on_start_document()
	{
		os_ << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
		os_ << "<!DOCTYPE cesAna SYSTEM \"xcesAnaIPI.dtd\">\n";
	}

	void PremorphProcessorImpl::on_start_element(const Glib::ustring &name, const AttributeList &attributes)
	{
		os_ << "<" << name;
		foreach (const xmlpp::SaxParser::Attribute& a, attributes) {
			os_ << " " << a.name << "=\"";
			encode_xml_entities_into(os_, a.value);
			os_ << "\"";
		}
		os_ << ">";
		os_ << "\n";
	}

	void PremorphProcessorImpl::on_end_element(const Glib::ustring &name)
	{
		UnicodeString s = UnicodeString::fromUTF8(buf_.str());
		s.trim();
		if (s.length() > 0) {
			tok_.reset();
			tok_.set_input_source(s);
			Toki::SentenceSplitter sen(tok_);
			while (sen.has_more()) {
				os_ << " <chunk type=\"s\">\n";
				boost::scoped_ptr<Sentence> sentence;
				sentence.reset(ma_.process_dispose(sen.get_next_sentence()));
				foreach (Token* t, sentence->tokens()) {
					token_as_xces_xml(os_, ma_.tagset(), *t, 1);
				}
				os_ << " </chunk>\n";
			}
			buf_.str("");
		}
		os_ << "</" << name << ">";
		os_ << "\n";
	}

	void PremorphProcessorImpl::on_characters(const Glib::ustring &text)
	{
		buf_ << static_cast<std::string>(text);
	}

	void PremorphProcessorImpl::on_cdata_block(const Glib::ustring & /*text*/)
	{

	}

	void PremorphProcessorImpl::on_error(const Glib::ustring &text)
	{
		std::cerr << "XML Error: " << (std::string)text << "\n";
	}
	void PremorphProcessorImpl::on_fatal_error(const Glib::ustring &text)
	{
		std::cerr << "XML Fatal error: " << (std::string)text << "\n";
	}

} /* end ns Maca */
