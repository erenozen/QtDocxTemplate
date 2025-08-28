#include "QtDocxTemplate/Docx.hpp"
#include "QtDocxTemplate/Variables.hpp"
#include "QtDocxTemplate/TextVariable.hpp"
#include "QtDocxTemplate/TableVariable.hpp"
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

int main(){QTemporaryDir tmp;assert(tmp.isValid());QString in=tmp.path()+"/tbl.docx";std::string body="<w:tbl><w:tr><w:tc><w:p><w:r><w:t>${students}</w:t></w:r></w:p></w:tc><w:tc><w:p><w:r><w:t>${students}</w:t></w:r></w:p></w:tc></w:tr></w:tbl>";writeMinimalDocx(in, makeDocXml(body));TableVariable tv("${students}");std::vector<VariablePtr> col1, col2;col1.push_back(std::make_shared<TextVariable>("n1","Alice"));col1.push_back(std::make_shared<TextVariable>("n2","Bob"));col2.push_back(std::make_shared<TextVariable>("a1","30"));col2.push_back(std::make_shared<TextVariable>("a2","25"));tv.addColumn(col1);tv.addColumn(col2);Variables vars;vars.add(std::make_shared<TableVariable>(tv));Docx d(in);d.fillTemplate(vars);QString out=in+".out.docx";d.save(out);Package pkg;assert(pkg.open(out));auto xml=pkg.readPart("word/document.xml");QString doc=*xml;assert(doc.contains("Alice"));assert(doc.contains("Bob"));assert(doc.contains("30"));assert(doc.contains("25"));std::cout<<"table_replace_test passed"<<std::endl;return 0;}
