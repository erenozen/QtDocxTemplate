#pragma once
#include "QtDocxTemplate/Export.hpp"
#include "QtDocxTemplate/VariablePattern.hpp"
#include "QtDocxTemplate/Variables.hpp"
#include <QString>
#include <QStringList>
#include <memory>
#include <optional>

namespace QtDocxTemplate {

// Forward declarations & internal includes (clean-room implementation)
namespace opc { class Package; }
namespace xml { class XmlPart; }

class QTDOCTXTEMPLATE_EXPORT Docx {
public:
    explicit Docx(QString templatePath);
    ~Docx();

    void setVariablePattern(const VariablePattern &pattern);
    QString readTextContent() const; // TODO Phase E implementation
    QStringList findVariables() const; // TODO Phase E implementation
    void fillTemplate(const Variables &variables); // TODO Phase E implementation
    void save(const QString &outputPath) const; // TODO Phase E implementation

private:
    QString m_templatePath;
    VariablePattern m_pattern;
    mutable std::shared_ptr<opc::Package> m_package; // OPC container (shared_ptr works with incomplete type)
    mutable bool m_openAttempted{false};
    mutable bool m_documentLoaded{false};
    bool ensureOpened() const; // lazy open helper
    bool ensureDocumentLoaded() const; // load word/document.xml once

    // Cached raw text lines of paragraphs (for findVariables re-use)
    QString readFullTextCache() const; // builds paragraph-joined text
};

} // namespace QtDocxTemplate
