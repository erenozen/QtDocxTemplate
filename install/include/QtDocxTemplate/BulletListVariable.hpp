/** \file BulletListVariable.hpp
 *  Bullet list placeholder; each item becomes a cloned paragraph.
 */
#pragma once
#include "QtDocxTemplate/Variable.hpp"
#include <vector>

namespace QtDocxTemplate {

/** Holds ordered list of item variables (currently text only for parity). */
class QTDOCTXTEMPLATE_EXPORT BulletListVariable : public Variable {
public:
    BulletListVariable(QString key) : Variable(std::move(key), VariableType::BulletList) {}
    /** Append an item (text variable expected). */
    void addItem(const VariablePtr &var) { m_items.push_back(var); }
    /** Access items in insertion order. */
    const std::vector<VariablePtr> & items() const { return m_items; }
private:
    std::vector<VariablePtr> m_items; // items can be text or image variables
};

} // namespace QtDocxTemplate
