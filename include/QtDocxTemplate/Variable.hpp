/** \file Variable.hpp
 *  Base type for all variable kinds (text, image, bullet list, table).
 */
#pragma once
#include <QString>
#include <memory>
#include "QtDocxTemplate/Export.hpp"

namespace QtDocxTemplate {

/** Discriminator for variable subtype. */
enum class VariableType { Text, Image, BulletList, Table };

/** Abstract base; holds placeholder key including prefix+suffix (e.g. "${name}"). */
class QTDOCTXTEMPLATE_EXPORT Variable {
public:
    explicit Variable(QString key, VariableType type) : m_key(std::move(key)), m_type(type) {}
    virtual ~Variable() = default;
    /** Placeholder token including delimiters. */
    const QString & key() const { return m_key; }
    /** Runtime type discriminator. */
    VariableType type() const { return m_type; }
private:
    QString m_key;
    VariableType m_type;
};

using VariablePtr = std::shared_ptr<Variable>;

} // namespace QtDocxTemplate
