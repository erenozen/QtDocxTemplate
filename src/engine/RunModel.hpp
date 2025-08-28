#pragma once
#include <QString>
#include <vector>
#include <functional>
#include <pugixml.hpp>

namespace QtDocxTemplate { namespace engine {

class RunModel {
public:
    struct Span {
        pugi::xml_node t; // <w:t>
        pugi::xml_node r; // parent <w:r>
        int start{0};
        int len{0};
    };

    void build(pugi::xml_node w_p); // build mapping for a paragraph
    const QString & text() const { return m_text; }
    pugi::xml_node styleSourceRun(int start, int end) const; // first overlapped run

    void replaceRange(int start, int end,
                      std::function<std::vector<pugi::xml_node>(pugi::xml_node w_p, pugi::xml_node styleR)> makeRuns);

    static pugi::xml_node cloneRunShallow(pugi::xml_node r);
    static pugi::xml_node makeTextRun(pugi::xml_node w_p, pugi::xml_node styleR, const QString &text, bool preserveSpace=true);

private:
    QString m_text;
    std::vector<Span> m_spans;
};

}} // namespace QtDocxTemplate::engine
