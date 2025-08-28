#include <QtDocxTemplate/Docx.hpp>
#include <QtDocxTemplate/TextVariable.hpp>
#include <QtDocxTemplate/Variables.hpp>
#include <iostream>

using namespace QtDocxTemplate;

int main() {
    Docx docx("template.docx");
    docx.setVariablePattern(VariablePattern{"${", "}"});
    Variables vars;
    vars.add(std::make_shared<TextVariable>("${firstname}", "Lukasz"));
    vars.add(std::make_shared<TextVariable>("${lastname}", "Stypka"));
    docx.fillTemplate(vars);
    docx.save("output.docx");
    std::cout << "Example (text variables) executed (stub)." << std::endl;
    return 0;
}
