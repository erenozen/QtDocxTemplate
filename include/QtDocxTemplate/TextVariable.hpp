#pragma once
#include "QtDocxTemplate/Variable.hpp"

namespace QtDocxTemplate {

class QTDOCTXTEMPLATE_EXPORT TextVariable : public Variable {
public:
    TextVariable(QString key, QString value)
        : Variable(std::move(key), VariableType::Text), m_value(std::move(value)) {}
    const QString & value() const { return m_value; }
private:
    QString m_value;
};

} // namespace QtDocxTemplate
