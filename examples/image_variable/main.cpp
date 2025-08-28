#include <QtDocxTemplate/Docx.hpp>
#include <QtDocxTemplate/ImageVariable.hpp>
#include <QtDocxTemplate/Variables.hpp>
#include <QImage>
#include <iostream>

using namespace QtDocxTemplate;

int main() {
    Docx docx("template_with_image.docx");
    Variables vars;
    QImage img(75,75,QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::red);
    vars.add(std::make_shared<ImageVariable>("${photo}", img, 75, 75));
    docx.fillTemplate(vars);
    docx.save("output_image.docx");
    std::cout << "Example (image variable) executed (stub)." << std::endl;
    return 0;
}
