#include <QtDocxTemplate/Docx.hpp>
#include <cassert>

using namespace QtDocxTemplate;

int main() {
    Docx d("dummy.docx");
    d.setVariablePattern(VariablePattern{"${","}"});
    // Smoke checks on stubs
    assert(d.readTextContent().isEmpty());
    assert(d.findVariables().isEmpty());
    return 0;
}
