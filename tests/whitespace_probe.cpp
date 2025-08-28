#include "QtDocxTemplate/Docx.hpp"
#include "QtDocxTemplate/Variables.hpp"
#include "QtDocxTemplate/TextVariable.hpp"
#include "opc/Package.hpp"
#include <cassert>
#include <QTemporaryDir>
#include <QFile>
#include <iostream>
using namespace QtDocxTemplate;
using QtDocxTemplate::opc::Package;
static std::string contentTypes(){return R"(<?xml version="1.0" encoding="UTF-8"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
  <Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
  <Default Extension="xml"  ContentType="application/xml"/>
  <Override PartName="/word/document.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/>
</Types>)"; }
static std::string rels(){return R"(<?xml version="1.0" encoding="UTF-8"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="word/document.xml"/>
</Relationships>)";}
int main(){QTemporaryDir tmp;assert(tmp.isValid());QString in=tmp.path()+"/e3.docx";std::string body="<w:p><w:r><w:t xml:space="preserve">  ${name}  </w:t></w:r></w:p>";std::string docxml="<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"><w:body>"+body+"</w:body></w:document>";Package pkg;pkg.writePart("[Content_Types].xml", QByteArray::fromStdString(contentTypes()));pkg.writePart("_rels/.rels", QByteArray::fromStdString(rels()));pkg.writePart("word/document.xml", QByteArray::fromStdString(docxml));bool ok=pkg.saveAs(in);assert(ok);Docx d(in);Variables vars;vars.add(std::make_shared<TextVariable>("name","Alice"));QString out=in+".out.docx";d.fillTemplate(vars);d.save(out);Package outPkg;outPkg.open(out);auto xml=outPkg.readPart("word/document.xml");std::cout<<xml->toStdString();}
