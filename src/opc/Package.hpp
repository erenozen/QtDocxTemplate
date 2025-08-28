#pragma once
#include <QString>
#include <QByteArray>
#include <vector>
#include <optional>

namespace QtDocxTemplate { namespace opc {

// TODO Phase A: Implement minimal OPC package handling using minizip/libzip
class Package {
public:
    bool open(const QString &path); // load template docx
    bool saveAs(const QString &path) const; // write new docx
    QByteArray readPart(const QString &name) const; // read /word/document.xml etc.
    void writePart(const QString &name, const QByteArray &data); // write modified part
    QString addMedia(const QByteArray &bytes, const QString &ext); // returns path like word/media/imageN.ext
private:
    // TODO store in-memory representation of zip entries
};

}} // namespace QtDocxTemplate::opc
