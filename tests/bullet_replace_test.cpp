#include "QtDocxTemplate/Docx.hpp"
#include "QtDocxTemplate/Variables.hpp"
#include "QtDocxTemplate/TextVariable.hpp"
#include "QtDocxTemplate/BulletListVariable.hpp"
#include "opc/Package.hpp"
#include <QTemporaryDir>
#include <cassert>
#include <iostream>

using namespace QtDocxTemplate; using QtDocxTemplate::opc::Package;

static std::string contentTypesMinimalWithDocument() {return R"(<?xml version="1.0" encoding="UTF-8"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
  <Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
  <Default Extension="xml"  ContentType="application/xml"/>
  <Override PartName="/word/document.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/>
</Types>)"; }
static std::string packageRelsMinimal(){return R"(<?xml version="1.0" encoding="UTF-8"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="word/document.xml"/>
</Relationships>)"; }
static std::string makeDocXml(const std::string &bodyXml){return std::string("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n")+"<w:document xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\"><w:body>"+bodyXml+"</w:body></w:document>"; }
static QString writeMinimalDocx(const QString &path,const std::string &docXml){Package pkg;pkg.writePart("[Content_Types].xml", QByteArray::fromStdString(contentTypesMinimalWithDocument()));pkg.writePart("_rels/.rels", QByteArray::fromStdString(packageRelsMinimal()));pkg.writePart("word/document.xml", QByteArray::fromStdString(docXml));bool ok=pkg.saveAs(path);assert(ok);return path;}

int main(){QTemporaryDir tmp;assert(tmp.isValid());QString in=tmp.path()+"/bul.docx";std::string body="<w:p><w:r><w:t>Skills: ${skills}</w:t></w:r></w:p>";writeMinimalDocx(in, makeDocXml(body));BulletListVariable bl("${skills}");bl.addItem(std::make_shared<TextVariable>("ignored","C++"));bl.addItem(std::make_shared<TextVariable>("ignored","Qt"));bl.addItem(std::make_shared<TextVariable>("ignored","DOCX"));Variables vars;vars.add(std::make_shared<BulletListVariable>(bl));Docx d(in);d.fillTemplate(vars);QString out=in+".out.docx";d.save(out);Package pkg;assert(pkg.open(out));auto xml=pkg.readPart("word/document.xml");QString doc=*xml;assert(doc.count("Skills:")==0);assert(doc.contains("C++"));assert(doc.contains("Qt"));assert(doc.contains("DOCX"));std::cout<<"bullet_replace_test passed"<<std::endl;return 0;}
