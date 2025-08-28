#pragma once
#include <QString>
#include <pugixml.hpp>
#include "QtDocxTemplate/Variables.hpp"

namespace QtDocxTemplate { namespace engine {

// TODO Phase D: Implement replacers for text, image, bullet list, table
struct Replacers {
    static void replaceText(pugi::xml_document &doc,
                            const QString &prefix,
                            const QString &suffix,
                            const ::QtDocxTemplate::Variables &vars);
    static void replaceImage(/* args TBD */) { /* TODO */ }
    static void replaceBulletList(/* args TBD */) { /* TODO */ }
    static void replaceTable(/* args TBD */) { /* TODO */ }
};

}} // namespace QtDocxTemplate::engine
