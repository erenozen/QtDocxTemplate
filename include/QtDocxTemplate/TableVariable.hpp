#pragma once
#include "QtDocxTemplate/Variable.hpp"
#include <vector>

namespace QtDocxTemplate {

class QTDOCTXTEMPLATE_EXPORT TableVariable : public Variable {
public:
    TableVariable(QString key) : Variable(std::move(key), VariableType::Table) {}
    // Column-wise lists of variables (text/image/bullet lists) representing each column's cells.
    void addColumn(const std::vector<VariablePtr> &column) { m_columns.push_back(column); }
    const std::vector<std::vector<VariablePtr>> & columns() const { return m_columns; }
private:
    std::vector<std::vector<VariablePtr>> m_columns; // column-wise data
};

} // namespace QtDocxTemplate
