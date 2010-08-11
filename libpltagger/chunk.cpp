#include <libpltagger/chunk.h>
#include <libtoki/util/foreach.h>

namespace PlTagger {

	Chunk::Chunk()
	{
	}

	Chunk::~Chunk()
	{
		foreach (Sentence* s, sentences_) {
			delete s;
		}
	}

	bool Chunk::has_attribute(const std::string &name) const
	{
		return attributes_.find(name) != attributes_.end();
	}

	std::string Chunk::get_attribute(const std::string &name) const
	{
		std::map<std::string, std::string>::const_iterator i;
		i = attributes_.find(name);
		if (i != attributes_.end()) {
			return i->second;
		} else {
			return "";
		}
	}

	void Chunk::set_attribute(const std::string &name, const std::string &value)
	{
		attributes_[name] = value;
	}

} /* end ns PlTagger */