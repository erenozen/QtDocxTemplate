#include "QtDocxTemplate/Docx.hpp"
#include <QFile>
#include "opc/Package.hpp"

namespace QtDocxTemplate {

bool Docx::ensureOpened() const {
    if(m_openAttempted) return m_package != nullptr;
    m_openAttempted = true;
    auto pkg = std::make_shared<opc::Package>();
    if(!pkg->open(m_templatePath)) {
        return false;
    }
    m_package = std::move(pkg);
    return true;
}

Docx::Docx(QString templatePath)
    : m_templatePath(std::move(templatePath)) {}

Docx::~Docx() = default;

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
    // Phase A: only ensure package is opened. No replacements yet.
    ensureOpened();
}

void Docx::save(const QString &outputPath) const {
    Q_UNUSED(outputPath);
    if(!ensureOpened()) return;
    if(m_package) {
        m_package->saveAs(outputPath);
    }
}

} // namespace QtDocxTemplate
