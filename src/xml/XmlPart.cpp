#include "xml/XmlPart.hpp"
#include <QByteArray>

namespace QtDocxTemplate { namespace xml {

bool XmlPart::load(const QByteArray &data) {
	m_doc.reset();
	pugi::xml_parse_result res = m_doc.load_buffer(data.constData(), data.size(), pugi::parse_default | pugi::parse_ws_pcdata);
	return res; // bool conversion indicates success
}

QByteArray XmlPart::save() const {
	QByteArray out;
	struct Writer : pugi::xml_writer {
		QByteArray *ba;
		explicit Writer(QByteArray *b):ba(b){}
		void write(const void *data, size_t size) override {
			ba->append(static_cast<const char*>(data), static_cast<int>(size));
		}
	} writer(&out);
	m_doc.save(writer, "  ", pugi::format_default, pugi::encoding_utf8);
	return out;
}

std::vector<pugi::xml_node> XmlPart::selectAll(const char* xpath) const {
	std::vector<pugi::xml_node> nodes;
	pugi::xpath_query q(xpath);
	pugi::xpath_node_set set = q.evaluate_node_set(m_doc);
	for(const auto &n : set) {
		nodes.push_back(n.node());
	}
	return nodes;
}

}} // namespace QtDocxTemplate::xml
