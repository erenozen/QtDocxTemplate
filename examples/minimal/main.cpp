#include <QtDocxTemplate/Docx.hpp>
#include <QtDocxTemplate/TextVariable.hpp>
#include <QtDocxTemplate/Variables.hpp>
#include <iostream>
using namespace QtDocxTemplate;
int main(){
    Docx doc("template.docx");
    Variables vars; vars.add(std::make_shared<TextVariable>("${name}", "Alice"));
    doc.fillTemplate(vars); doc.save("out.docx");
    std::cout << "Minimal example built" << std::endl; return 0;
}
