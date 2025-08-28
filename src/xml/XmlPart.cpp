#include "xml/XmlPart.hpp"

namespace QtDocxTemplate { namespace xml {

bool XmlPart::load(const QByteArray &data) { (void)data; return true; }
QByteArray XmlPart::serialize() const { return {}; }
QString XmlPart::textContent() const { return {}; }

}} // namespace QtDocxTemplate::xml
