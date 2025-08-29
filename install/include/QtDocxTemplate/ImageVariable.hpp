/** \file ImageVariable.hpp
 *  Image placeholder; inserted as a drawing run (main document only).
 */
#pragma once
#include "QtDocxTemplate/Variable.hpp"
#include <QImage>

namespace QtDocxTemplate {

/** Stores an image and desired pixel size (converted to EMU @96DPI). */
class QTDOCTXTEMPLATE_EXPORT ImageVariable : public Variable {
public:
    ImageVariable(QString key, QImage image, int widthPx, int heightPx)
        : Variable(std::move(key), VariableType::Image), m_image(std::move(image)), m_width(widthPx), m_height(heightPx) {}
    /** Raw QImage copied into package media/ directory. */
    const QImage & image() const { return m_image; }
    /** Target width in pixels (not auto-scaled). */
    int widthPx() const { return m_width; }
    /** Target height in pixels. */
    int heightPx() const { return m_height; }
private:
    QImage m_image;
    int m_width{0};
    int m_height{0};
};

} // namespace QtDocxTemplate
