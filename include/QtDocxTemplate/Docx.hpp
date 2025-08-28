#pragma once
#include "QtDocxTemplate/Export.hpp"
#include "QtDocxTemplate/VariablePattern.hpp"
#include "QtDocxTemplate/Variables.hpp"
#include <QString>
#include <QStringList>
#include <memory>

namespace QtDocxTemplate {

// Forward declarations of internal components (clean-room implementation)
namespace opc { class Package; }
namespace xml { class XmlPart; }

class QTDOCTXTEMPLATE_EXPORT Docx {
public:
    explicit Docx(QString templatePath);

    void setVariablePattern(const VariablePattern &pattern);
    QString readTextContent() const; // TODO Phase E implementation
    QStringList findVariables() const; // TODO Phase E implementation
    void fillTemplate(const Variables &variables); // TODO Phase E implementation
    void save(const QString &outputPath) const; // TODO Phase E implementation

private:
    QString m_templatePath;
    VariablePattern m_pattern;
    // TODO store internal model / parsed document once implemented
};

} // namespace QtDocxTemplate
