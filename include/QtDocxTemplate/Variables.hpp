#pragma once
#include "QtDocxTemplate/Variable.hpp"
#include <vector>

namespace QtDocxTemplate {

class QTDOCTXTEMPLATE_EXPORT Variables {
public:
    void add(const VariablePtr &v); // see implementation
    const std::vector<VariablePtr> & all() const { return m_vars; }
private:
    std::vector<VariablePtr> m_vars;
};

} // namespace QtDocxTemplate
