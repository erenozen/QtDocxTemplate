// Standalone assert-based test (no gtest dependency) similar to other phase tests
#include "QtDocxTemplate/Docx.hpp"
#include "QtDocxTemplate/Variables.hpp"
#include "QtDocxTemplate/TextVariable.hpp"
#include "opc/Package.hpp"
#include "xml/XmlPart.hpp"
#include <QTemporaryDir>
#include <QFile>
#include <cassert>
#include <iostream>

using namespace QtDocxTemplate;

static QByteArray minimalDocxWithContent(const QString &paraXml) {
    opc::Package pkg;
    pkg.writePart("[Content_Types].xml", QByteArray(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" \
        "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">" \
        "<Default Extension=\"xml\" ContentType=\"application/xml\"/>" \
        "<Override PartName=\"/word/document.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml\"/>" \
        "</Types>"));
    QByteArray docXml = QByteArray("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" \
        "<w:document xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" xmlns:xml=\"http://www.w3.org/XML/1998/namespace\">" \
        "<w:body>" + paraXml.toUtf8() + "</w:body></w:document>");
    pkg.writePart("word/document.xml", docXml);
    QTemporaryDir dir; QString outPath = dir.path()+"/doc.docx"; pkg.saveAs(outPath); QFile f(outPath); assert(f.open(QIODevice::ReadOnly)); return f.readAll();
}

int main(){
    // Simple single run
    {
        QString para = "<w:p><w:r><w:t>${NAME}</w:t></w:r></w:p>";
        QByteArray docBytes = minimalDocxWithContent(para);
        QTemporaryDir dir; QString path = dir.path()+"/t.docx"; QFile f(path); assert(f.open(QIODevice::WriteOnly)); f.write(docBytes); f.close();
        Docx d(path); Variables vars; vars.add(std::make_shared<TextVariable>("NAME","Alice")); d.fillTemplate(vars); d.save(path+"out.docx");
        Docx d2(path+"out.docx"); QString text = d2.readTextContent(); assert(text.contains("Alice"));
    }
    // Split runs variable
    {
        QString para = "<w:p><w:r><w:t>$</w:t></w:r><w:r><w:t>{NA</w:t></w:r><w:r><w:t>ME}</w:t></w:r></w:p>";
        QByteArray docBytes = minimalDocxWithContent(para);
        QTemporaryDir dir; QString path = dir.path()+"/t.docx"; QFile f(path); assert(f.open(QIODevice::WriteOnly)); f.write(docBytes); f.close();
        Docx d(path); Variables vars; vars.add(std::make_shared<TextVariable>("NAME","Bob")); d.fillTemplate(vars); d.save(path+"out.docx");
    Docx d2(path+"out.docx"); QString text = d2.readTextContent();
    // Expect single paragraph replaced fully with Bob (no leftover braces)
    assert(!text.contains("${NAME}"));
    assert(text.contains("Bob"));
    }
    // Unknown variable untouched
    {
        QString para = "<w:p><w:r><w:t>${KNOWN}</w:t></w:r><w:r><w:t>${UNKNOWN}</w:t></w:r></w:p>";
        QByteArray docBytes = minimalDocxWithContent(para);
        QTemporaryDir dir; QString path = dir.path()+"/t.docx"; QFile f(path); assert(f.open(QIODevice::WriteOnly)); f.write(docBytes); f.close();
        Docx d(path); Variables vars; vars.add(std::make_shared<TextVariable>("KNOWN","X")); d.fillTemplate(vars); d.save(path+"out.docx");
        Docx d2(path+"out.docx"); QString text = d2.readTextContent(); assert(text.contains("X")); assert(text.contains("${UNKNOWN}"));
    }
    // Multiple descending order
    {
        QString para = "<w:p><w:r><w:t>${A} ${AB}</w:t></w:r></w:p>";
        QByteArray docBytes = minimalDocxWithContent(para);
        QTemporaryDir dir; QString path = dir.path()+"/t.docx"; QFile f(path); assert(f.open(QIODevice::WriteOnly)); f.write(docBytes); f.close();
        Docx d(path); Variables vars; vars.add(std::make_shared<TextVariable>("A","one")); vars.add(std::make_shared<TextVariable>("AB","two")); d.fillTemplate(vars); d.save(path+"out.docx");
        Docx d2(path+"out.docx"); QString text = d2.readTextContent(); assert(text.contains("one two"));
    }
    // Style preservation
    {
        QString para = "<w:p><w:r><w:rPr><w:b/></w:rPr><w:t>${NAME}</w:t></w:r></w:p>";
        QByteArray docBytes = minimalDocxWithContent(para);
        QTemporaryDir dir; QString path = dir.path()+"/t.docx"; QFile f(path); assert(f.open(QIODevice::WriteOnly)); f.write(docBytes); f.close();
        Docx d(path); Variables vars; vars.add(std::make_shared<TextVariable>("NAME","Styled")); d.fillTemplate(vars); d.save(path+"out.docx");
        opc::Package pkg; assert(pkg.open(path+"out.docx")); auto docPart = pkg.readPart("word/document.xml"); assert(docPart.has_value());
        xml::XmlPart xp; assert(xp.load(*docPart));
        auto nodes = xp.selectAll("//w:r[w:rPr]/w:t");
        bool found=false; for(auto &n : nodes){ if(QString::fromUtf8(n.text().get())=="Styled") found=true; }
        assert(found);
    }
    std::cout << "text_replace_test passed" << std::endl; return 0;
}
