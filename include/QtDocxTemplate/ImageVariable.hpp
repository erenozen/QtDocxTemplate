#pragma once
#include "QtDocxTemplate/Variable.hpp"
#include <QImage>

namespace QtDocxTemplate {

class QTDOCTXTEMPLATE_EXPORT ImageVariable : public Variable {
public:
    ImageVariable(QString key, QImage image, int widthPx, int heightPx)
        : Variable(std::move(key), VariableType::Image), m_image(std::move(image)), m_width(widthPx), m_height(heightPx) {}
    const QImage & image() const { return m_image; }
    int widthPx() const { return m_width; }
    int heightPx() const { return m_height; }
private:
    QImage m_image;
    int m_width{0};
    int m_height{0};
};

} // namespace QtDocxTemplate
