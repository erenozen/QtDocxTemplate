#include "QtDocxTemplate/Docx.hpp"
#include "QtDocxTemplate/Variables.hpp"
#include "QtDocxTemplate/ImageVariable.hpp"
#include "opc/Package.hpp"
#include <QTemporaryDir>
#include <QImage>
#include <QBuffer>
#include <cassert>
#include <iostream>

using namespace QtDocxTemplate;
using QtDocxTemplate::opc::Package;

static std::string contentTypesMinimalWithDocument() {
    return R"(<?xml version="1.0" encoding="UTF-8"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
  <Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
  <Default Extension="xml"  ContentType="application/xml"/>
  <Override PartName="/word/document.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/>
</Types>)"; }

static std::string packageRelsMinimal() {
    return R"(<?xml version="1.0" encoding="UTF-8"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="word/document.xml"/>
</Relationships>)"; }

static std::string makeDocXml(const std::string &bodyXml) {
    return std::string("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n") +
           "<w:document xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\">" +
           "<w:body>" + bodyXml + "</w:body></w:document>"; }

static QString writeMinimalDocx(const QString &path, const std::string &documentXml) {
    Package pkg; pkg.writePart("[Content_Types].xml", QByteArray::fromStdString(contentTypesMinimalWithDocument()));
    pkg.writePart("_rels/.rels", QByteArray::fromStdString(packageRelsMinimal()));
    pkg.writePart("word/document.xml", QByteArray::fromStdString(documentXml));
    bool ok = pkg.saveAs(path); assert(ok); return path;
}

int main(){
    QTemporaryDir tmp; assert(tmp.isValid());
    QString in = tmp.path()+"/img.docx";
    std::string body = "<w:p><w:r><w:t>Photo: ${photo}</w:t></w:r></w:p>";
    writeMinimalDocx(in, makeDocXml(body));
    QImage img(75,75,QImage::Format_ARGB32); img.fill(Qt::red);
    Variables vars; vars.add(std::make_shared<ImageVariable>("${photo}", img, 75, 75));
    Docx d(in); d.fillTemplate(vars); QString out = in+".out.docx"; d.save(out);
    Package pkg; assert(pkg.open(out)); auto docXml = pkg.readPart("word/document.xml"); assert(docXml->contains("w:drawing"));
    assert(docXml->contains("r:embed=\"rId"));
    auto rels = pkg.readPart("word/_rels/document.xml.rels"); assert(rels.has_value()); assert(rels->contains("image"));
    // extent check
    const int emu = (int)(75* (914400.0/96.0) + 0.5); std::string needle = std::string("cx=\"")+std::to_string(emu)+"\""; assert(docXml->contains(QByteArray(needle.c_str())));
    std::cout << "image_replace_test passed" << std::endl; return 0;
}
