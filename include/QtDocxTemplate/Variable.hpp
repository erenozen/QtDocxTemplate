#pragma once
#include <QString>
#include <memory>
#include "QtDocxTemplate/Export.hpp"

namespace QtDocxTemplate {

enum class VariableType {
    Text,
    Image,
    BulletList,
    Table
};

class QTDOCTXTEMPLATE_EXPORT Variable {
public:
    explicit Variable(QString key, VariableType type) : m_key(std::move(key)), m_type(type) {}
    virtual ~Variable() = default;
    const QString & key() const { return m_key; }
    VariableType type() const { return m_type; }
private:
    QString m_key;
    VariableType m_type;
};

using VariablePtr = std::shared_ptr<Variable>;

} // namespace QtDocxTemplate
