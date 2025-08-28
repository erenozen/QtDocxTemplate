/** \file VariablePattern.hpp
 *  Defines prefix/suffix delimiters (default ${ }). Used verbatim in keys.
 */
#pragma once
#include <QString>

namespace QtDocxTemplate {

/** Placeholder delimiters; change via Docx::setVariablePattern before fill. */
struct VariablePattern {
    QString prefix{"${"}; ///< Leading token marker
    QString suffix{"}"}; ///< Trailing token marker
};

} // namespace QtDocxTemplate
