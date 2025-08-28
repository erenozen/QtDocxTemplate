#include "QtDocxTemplate/Docx.hpp"
#include <QFile>

namespace QtDocxTemplate {

Docx::Docx(QString templatePath)
    : m_templatePath(std::move(templatePath)) {}

void Docx::setVariablePattern(const VariablePattern &pattern) {
    m_pattern = pattern;
}

QString Docx::readTextContent() const {
    // TODO Phase E: extract visible text from document.xml
    return {};
}

QStringList Docx::findVariables() const {
    // TODO Phase E: scan runs for variable patterns
    return {};
}

void Docx::fillTemplate(const Variables &variables) {
    Q_UNUSED(variables);
    // TODO Phase E: orchestrate replacement using RunModel + Replacers
}

void Docx::save(const QString &outputPath) const {
    Q_UNUSED(outputPath);
    // TODO Phase E: write modified package to outputPath
}

} // namespace QtDocxTemplate
