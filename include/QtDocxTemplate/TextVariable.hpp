/** \file TextVariable.hpp
 *  Text replacement variable; style taken from first intersecting run.
 */
#pragma once
#include "QtDocxTemplate/Variable.hpp"

namespace QtDocxTemplate {

/** Holds literal replacement text content. */
class QTDOCTXTEMPLATE_EXPORT TextVariable : public Variable {
public:
    TextVariable(QString key, QString value)
        : Variable(std::move(key), VariableType::Text), m_value(std::move(value)) {}
    /** Replacement text (no XML markup). */
    const QString & value() const { return m_value; }
private:
    QString m_value;
};

} // namespace QtDocxTemplate
