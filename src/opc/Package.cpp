#include "opc/Package.hpp"

namespace QtDocxTemplate { namespace opc {

bool Package::open(const QString &path) { (void)path; return true; }
bool Package::saveAs(const QString &path) const { (void)path; return true; }
QByteArray Package::readPart(const QString &name) const { (void)name; return {}; }
void Package::writePart(const QString &name, const QByteArray &data) { (void)name; (void)data; }
QString Package::addMedia(const QByteArray &bytes, const QString &ext) { (void)bytes; (void)ext; return QStringLiteral("word/media/image1") + ext; }

}} // namespace QtDocxTemplate::opc
