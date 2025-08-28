/** \file TableVariable.hpp
 *  Table placeholder replicating rows based on column-wise variable lists.
 */
#pragma once
#include "QtDocxTemplate/Variable.hpp"
#include <vector>

namespace QtDocxTemplate {

/** Represents a table data set: each column is a vector of equal length. */
class QTDOCTXTEMPLATE_EXPORT TableVariable : public Variable {
public:
    TableVariable(QString key) : Variable(std::move(key), VariableType::Table) {}
    /** Add a column (size must match other columns for full replication). */
    void addColumn(const std::vector<VariablePtr> &column) { m_columns.push_back(column); }
    /** Column-wise collection; row count is size of first column. */
    const std::vector<std::vector<VariablePtr>> & columns() const { return m_columns; }
private:
    std::vector<std::vector<VariablePtr>> m_columns; // column-wise data
};

} // namespace QtDocxTemplate
