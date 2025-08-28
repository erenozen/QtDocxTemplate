#pragma once
#include "QtDocxTemplate/Variable.hpp"
#include <vector>

namespace QtDocxTemplate {

class QTDOCTXTEMPLATE_EXPORT BulletListVariable : public Variable {
public:
    BulletListVariable(QString key) : Variable(std::move(key), VariableType::BulletList) {}
    void addItem(const VariablePtr &var) { m_items.push_back(var); }
    const std::vector<VariablePtr> & items() const { return m_items; }
private:
    std::vector<VariablePtr> m_items; // items can be text or image variables
};

} // namespace QtDocxTemplate
