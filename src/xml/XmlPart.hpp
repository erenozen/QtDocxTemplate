#pragma once
#include <QString>
#include <QByteArray>

namespace QtDocxTemplate { namespace xml {

// TODO Phase B: Wrap pugixml document, provide namespace registrations and helpers
class XmlPart {
public:
    bool load(const QByteArray &data); // parse
    QByteArray serialize() const; // to bytes
    QString textContent() const; // flatten visible text
    // Additional helper methods will be added in later phases
};

}} // namespace QtDocxTemplate::xml
