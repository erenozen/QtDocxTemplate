/** \file Variables.hpp
 *  Container of variable objects supplied to Docx::fillTemplate.
 */
#pragma once
#include "QtDocxTemplate/Variable.hpp"
#include <vector>

namespace QtDocxTemplate {

/** Simple aggregate of shared_ptr<Variable>. Order preserved. */
class QTDOCTXTEMPLATE_EXPORT Variables {
public:
    /** Append a variable (no deduplication performed). */
    void add(const VariablePtr &v);
    /** Access underlying ordered collection. */
    const std::vector<VariablePtr> & all() const { return m_vars; }
private:
    std::vector<VariablePtr> m_vars;
};

} // namespace QtDocxTemplate
