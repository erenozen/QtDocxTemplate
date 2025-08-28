#pragma once
#include <QString>

namespace QtDocxTemplate {

struct VariablePattern {
    QString prefix{"${"};
    QString suffix{"}"};
};

} // namespace QtDocxTemplate
