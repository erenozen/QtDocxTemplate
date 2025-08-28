#include "QtDocxTemplate/Variables.hpp"

namespace QtDocxTemplate {

void Variables::add(const VariablePtr &v) {
    m_vars.push_back(v);
}

} // namespace QtDocxTemplate
