// Edge text replacement tests – no framework, use asserts
#include "QtDocxTemplate/Docx.hpp"
#include "QtDocxTemplate/Variables.hpp"
#include "QtDocxTemplate/TextVariable.hpp"
#include "opc/Package.hpp"
#include "xml/XmlPart.hpp"
#include <QTemporaryDir>
#include <QFile>
#include <QDir>
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

static pugi::xml_document loadXmlFromPart(Package &pkg, const char* partName) {
    pugi::xml_document d; auto data = pkg.readPart(partName); assert(data.has_value());
    pugi::xml_parse_result r = d.load_buffer(data->constData(), data->size()); assert(r); return d;
}

static bool containsXPath(const pugi::xml_document &doc, const char* xpath) {
    pugi::xpath_query q(xpath); auto ns = q.evaluate_node_set(doc); return !ns.empty();
}

int main() {
    QTemporaryDir tmp; assert(tmp.isValid()); QString base = tmp.path();

    // E1 Style precedence (bold then italic parts of placeholder)
    {
        std::string body = R"(<w:p>
  <w:r><w:rPr><w:b/></w:rPr><w:t>${na</w:t></w:r>
  <w:r><w:rPr><w:i/></w:rPr><w:t>me}</w:t></w:r>
</w:p>)";
        QString inPath = base+"/e1.docx"; writeMinimalDocx(inPath, makeDocXml(body));
    Docx doc(inPath); Variables vars; vars.add(std::make_shared<TextVariable>("name","Alice")); QString outPath = inPath+".out.docx"; doc.fillTemplate(vars); doc.save(outPath);
    Package pkg; assert(pkg.open(outPath)); auto d = loadXmlFromPart(pkg, "word/document.xml");
        // Replacement run must have <w:b/> and not <w:i/>
        assert(containsXPath(d, "//w:r[w:rPr/w:b]/w:t[text()='Alice']"));
        assert(!containsXPath(d, "//w:r[w:rPr/w:i]/w:t[text()='Alice']"));
        // Ensure no leftover braces
        auto docBytes = pkg.readPart("word/document.xml"); assert(docBytes->contains("Alice")); assert(!docBytes->contains("${na"));
    }

    // E2 Boundary positions (start, end, adjacent)
    {
        std::string body = R"(<w:p><w:r><w:t>${greet} world</w:t></w:r></w:p>
<w:p><w:r><w:t>Hi ${name}</w:t></w:r></w:p>
<w:p><w:r><w:t>${a}${b}</w:t></w:r></w:p>)";
        QString inPath = base+"/e2.docx"; writeMinimalDocx(inPath, makeDocXml(body));
    Docx doc(inPath); Variables vars; vars.add(std::make_shared<TextVariable>("greet","Hello")); vars.add(std::make_shared<TextVariable>("name","Alice")); vars.add(std::make_shared<TextVariable>("a","A")); vars.add(std::make_shared<TextVariable>("b","B"));
    QString outPath = inPath + ".out.docx"; doc.fillTemplate(vars); doc.save(outPath); Docx verify(outPath); QString text = verify.readTextContent();
    QStringList lines = text.split('\n'); assert(lines.size()==3); auto l0=lines[0].trimmed(); auto l1=lines[1].trimmed(); auto l2=lines[2].trimmed();
    assert(l0=="Hello world"); assert(l1=="Hi Alice"); assert(l2=="AB");
        assert(!text.contains("${"));
    }

    // E3 Whitespace preservation
    {
        std::string body = R"(<w:p><w:r><w:t xml:space="preserve">  ${name}  </w:t></w:r></w:p>)";
        QString inPath = base+"/e3.docx"; writeMinimalDocx(inPath, makeDocXml(body));
    Docx doc(inPath); Variables vars; vars.add(std::make_shared<TextVariable>("name","Alice")); QString outPath = inPath+".out.docx"; doc.fillTemplate(vars); doc.save(outPath);
    Package pkg; assert(pkg.open(outPath)); auto d = loadXmlFromPart(pkg, "word/document.xml");
        // Text exact
        assert(containsXPath(d, "//w:t[@xml:space='preserve' and text()='  Alice  ' ]"));
    }

    // E4 Unknown key remains
    {
        std::string body = R"(<w:p><w:r><w:t>X ${missing} Y</w:t></w:r></w:p>)";
        QString inPath = base+"/e4.docx"; writeMinimalDocx(inPath, makeDocXml(body));
    Docx doc(inPath); Variables vars; vars.add(std::make_shared<TextVariable>("name","Alice")); QString outPath = inPath+".out.docx"; doc.fillTemplate(vars); doc.save(outPath);
    Package pkg; assert(pkg.open(outPath)); auto part = pkg.readPart("word/document.xml"); assert(part->contains("${missing}"));
    }

    // E5 Repeated key occurrences across paragraphs
    {
        std::string body = R"(<w:p><w:r><w:t>Hi ${name}, ${name}</w:t></w:r></w:p>
<w:p><w:r><w:t>Bye ${name}</w:t></w:r></w:p>)";
        QString inPath = base+"/e5.docx"; writeMinimalDocx(inPath, makeDocXml(body));
    Docx doc(inPath); Variables vars; vars.add(std::make_shared<TextVariable>("name","Alice")); QString outPath = inPath+".out.docx"; doc.fillTemplate(vars); doc.save(outPath);
    Docx verify(outPath); QString text = verify.readTextContent(); QStringList lines = text.split('\n'); assert(lines.size()==2); assert(lines[0]=="Hi Alice, Alice"); assert(lines[1]=="Bye Alice");
    }

    std::cout << "text_replace_edge_test passed" << std::endl; return 0;
}
