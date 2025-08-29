#pragma once
#include <QString>
#include <QByteArray>
#include <vector>
#include <pugixml.hpp>

namespace QtDocxTemplate { namespace xml {

// XML document wrapper providing namespace handling and XPath queries
class XmlPart {
public:
    bool load(const QByteArray &data); // parse; false on failure
    QByteArray save() const;           // serialize UTF-8 with XML decl
    std::vector<pugi::xml_node> selectAll(const char* xpath) const; // basic xpath queries
    pugi::xml_document & doc() { return m_doc; }
    const pugi::xml_document & doc() const { return m_doc; }
private:
    pugi::xml_document m_doc;
};

}} // namespace QtDocxTemplate::xml
