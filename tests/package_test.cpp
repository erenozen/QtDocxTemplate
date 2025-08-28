#include <QtDocxTemplate/Docx.hpp>
#include "opc/Package.hpp"
#include <QTemporaryDir>
#include <QFile>
#include <QDir>
#include <cassert>
#include <iostream>

using namespace QtDocxTemplate;
using QtDocxTemplate::opc::Package;

static void writeFile(const QString &path, const QByteArray &data) {
  QDir dir(QFileInfo(path).path());
  dir.mkpath(".");
  QFile f(path);
  if(!f.open(QIODevice::WriteOnly)) {
    std::cerr << "Failed to write file: " << path.toStdString() << std::endl;
    std::abort();
  }
  f.write(data);
  f.close();
}

int main() {
    QTemporaryDir tmp;
    assert(tmp.isValid());
    QString base = tmp.path();

    // Minimal OPC structure
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
    QByteArray document = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main">
  <w:body><w:p><w:r><w:t>Hello</w:t></w:r></w:p></w:body>
</w:document>)";

    // Use Package to assemble
    Package pkg;
    pkg.writePart("[Content_Types].xml", contentTypes);
    pkg.writePart("_rels/.rels", rels);
    pkg.writePart("word/document.xml", document);
    QString inputDocx = base + "/test_input.docx";
    assert(pkg.saveAs(inputDocx));

    // Re-open and verify
    Package pkg2;
    assert(pkg2.open(inputDocx));
    auto docPart = pkg2.readPart("word/document.xml");
    assert(docPart && !docPart->isEmpty());

    // Add custom part and round-trip
    pkg2.writePart("word/custom.xml", QByteArray("<test/>") );
    QString roundTrip = base + "/roundtrip.docx";
    assert(pkg2.saveAs(roundTrip));

    Package pkg3; assert(pkg3.open(roundTrip));
    auto custom = pkg3.readPart("word/custom.xml");
    assert(custom.has_value());

    // Test addMedia increments and updates content types
    QByteArray dummyImg(10, '\0');
    QString m1 = pkg3.addMedia(dummyImg, "png");
    QString m2 = pkg3.addMedia(dummyImg, "png");
    assert(m1 != m2);
    auto ct = pkg3.readPart("[Content_Types].xml");
    assert(ct && ct->contains("image/png"));

    std::cout << "package_test passed" << std::endl;
    return 0;
}
