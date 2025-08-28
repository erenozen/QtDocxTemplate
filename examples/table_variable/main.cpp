#include <QtDocxTemplate/Docx.hpp>
#include <QtDocxTemplate/TableVariable.hpp>
#include <QtDocxTemplate/TextVariable.hpp>
#include <QtDocxTemplate/Variables.hpp>
#include <iostream>

using namespace QtDocxTemplate;

int main() {
    Docx docx("template_table.docx");
    TableVariable table("${table}");
    // Simulate two columns with two rows each (stub data)
    table.addColumn({ std::make_shared<TextVariable>("${col1row1}", "A1"), std::make_shared<TextVariable>("${col1row2}", "A2") });
    table.addColumn({ std::make_shared<TextVariable>("${col2row1}", "B1"), std::make_shared<TextVariable>("${col2row2}", "B2") });
    Variables vars;
    vars.add(std::make_shared<TableVariable>(table)); // copy
    docx.fillTemplate(vars);
    docx.save("output_table.docx");
    std::cout << "Example (table variable) executed (stub)." << std::endl;
    return 0;
}
