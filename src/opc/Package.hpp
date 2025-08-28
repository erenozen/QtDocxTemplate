#pragma once
#include <QString>
#include <QByteArray>
#include <QHash>
#include <optional>
#include <QStringList>

namespace QtDocxTemplate { namespace opc {

// TODO Phase A: Implement minimal OPC package handling using minizip/libzip
// Simple in-memory representation of a DOCX OPC package (ZIP) providing parity-level
// operations required for later replacement phases. Not a full OPC implementation.
class Package {
public:
    bool open(const QString &path);              // Load .docx (ZIP) into memory map
    bool saveAs(const QString &path) const;       // Write current parts to .docx
    std::optional<QByteArray> readPart(const QString &name) const; // Get part bytes if present
    void writePart(const QString &name, const QByteArray &data);   // Create/overwrite a part
    QString addMedia(const QByteArray &bytes, const QString &ext); // Adds media file and returns its part name

    bool hasPart(const QString &name) const { return m_parts.contains(name); }
    QStringList partNames() const { return m_parts.keys(); }

private:
    QHash<QString,QByteArray> m_parts; // partName -> data (always using forward slashes)
    int nextImageIndex(const QString &ext) const; // scan existing media for next index
    void ensureDefaultContentType(const QString &ext, const QString &mime); // update [Content_Types].xml
    void normalizePath(QString &p) const; // ensure forward slashes, no leading ./
};

}} // namespace QtDocxTemplate::opc
