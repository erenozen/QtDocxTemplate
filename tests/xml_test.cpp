#include <QtDocxTemplate/Docx.hpp>
#include "opc/Package.hpp"
#include "xml/XmlPart.hpp"
#include <cassert>
#include <iostream>
#include <QDir>
#include <QStandardPaths>

using namespace QtDocxTemplate;
using QtDocxTemplate::opc::Package;

int main() {
    // Test A: XmlPart selectAll
    {
        QByteArray xml = R"(<?xml version="1.0"?><w:root xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"><w:p><w:r><w:t>A</w:t></w:r><w:r><w:t>B</w:t></w:r></w:p></w:root>)";
        xml::XmlPart part; assert(part.load(xml));
        auto nodes = part.selectAll("//w:t");
        assert(nodes.size() == 2);
    }

    // Test B: Docx readTextContent + findVariables (default and custom pattern)
    QByteArray contentTypes = R"(<?xml version="1.0" encoding="UTF-8"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
  <Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
  <Default Extension="xml"  ContentType="application/xml"/>
  <Override PartName="/word/document.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/>
</Types>)";
    QByteArray rels = R"(<?xml version="1.0" encoding="UTF-8"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="word/document.xml"/>
</Relationships>)";
    // Paragraph 1: Hello ${name}!
    // Paragraph 2: split ${firstname}
    // Paragraph 3: custom pattern #{age}
    QByteArray document = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main">
  <w:body>
    <w:p><w:r><w:t>Hello ${name}!</w:t></w:r></w:p>
    <w:p><w:r><w:t>${fir</w:t></w:r><w:r><w:t>stname}</w:t></w:r></w:p>
    <w:p><w:r><w:t>#{age}</w:t></w:r></w:p>
  </w:body>
</w:document>)";

    Package pkg;
    pkg.writePart("[Content_Types].xml", contentTypes);
    pkg.writePart("_rels/.rels", rels);
    pkg.writePart("word/document.xml", document);
  QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
  if(tempDir.isEmpty()) tempDir = ".";
  QString path = tempDir + "/phaseb_test.docx";
    assert(pkg.saveAs(path));

    Docx docx(path);
    QString text = docx.readTextContent();
    QStringList lines = text.split('\n');
    assert(lines.size() == 3);
    auto varsDefault = docx.findVariables();
    assert(varsDefault.contains("${name}"));
    assert(varsDefault.contains("${firstname}"));
    // Switch pattern and re-scan
    docx.setVariablePattern(VariablePattern{"#{", "}"});
    auto varsCustom = docx.findVariables();
    assert(!varsCustom.contains("${name}"));
    assert(varsCustom.size() == 1 && varsCustom.first() == "#{age}");

    std::cout << "xml_test passed" << std::endl;
    return 0;
}
