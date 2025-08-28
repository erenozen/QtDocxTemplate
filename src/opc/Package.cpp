#include "opc/Package.hpp"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QRegularExpression>
#include <QDebug>
#include <cstring>

#ifdef QTDOCTEMPLATE_USE_LIBZIP
#include <zip.h>
#else
// minizip-ng
#include <mz.h>
#include <mz_os.h>
#include <mz_strm.h>
#include <mz_strm_mem.h>
#include <mz_strm_buf.h>
#include <zip.h> // compatibility layer
#include <unzip.h>
#include <zlib.h>
#endif

namespace QtDocxTemplate { namespace opc {

void Package::normalizePath(QString &p) const {
	p.replace('\\','/');
	if(p.startsWith("./")) p.remove(0,2);
}

bool Package::open(const QString &path) {
	m_parts.clear();
#ifdef QTDOCTEMPLATE_USE_LIBZIP
	int err = 0;
	zip_t *archive = zip_open(path.toUtf8().constData(), ZIP_RDONLY, &err);
	if(!archive) {
		qWarning() << "libzip: cannot open" << path << "err" << err;
		return false;
	}
	zip_int64_t num = zip_get_num_entries(archive, 0);
	for(zip_uint64_t i=0;i<(zip_uint64_t)num;++i) {
		struct zip_stat st; zip_stat_init(&st);
		if(zip_stat_index(archive, i, 0, &st)==0) {
			if(st.name && st.size >= 0) {
				zip_file_t *zf = zip_fopen_index(archive, i, 0);
				if(!zf) continue;
				QByteArray data; data.resize(static_cast<int>(st.size));
				zip_int64_t rd = zip_fread(zf, data.data(), st.size);
				zip_fclose(zf);
				if(rd == (zip_int64_t)st.size) {
					QString name = QString::fromUtf8(st.name);
					normalizePath(name);
					m_parts.insert(name, data);
				}
			}
		}
	}
	zip_close(archive);
	return true;
#else
	unzFile uf = unzOpen(path.toUtf8().constData());
	if(!uf) return false;
	if(unzGoToFirstFile(uf) != UNZ_OK) { unzClose(uf); return false; }
	do {
		char filename[512];
		unz_file_info64 info{};
		if(unzGetCurrentFileInfo64(uf, &info, filename, sizeof(filename), nullptr, 0, nullptr, 0) != UNZ_OK) break;
		if(unzOpenCurrentFile(uf) != UNZ_OK) break;
		QByteArray data; data.resize((int)info.uncompressed_size);
		int rd = unzReadCurrentFile(uf, data.data(), data.size());
		unzCloseCurrentFile(uf);
		if(rd == data.size()) {
			QString name = QString::fromUtf8(filename);
			normalizePath(name);
			m_parts.insert(name, data);
		}
	} while(unzGoToNextFile(uf) == UNZ_OK);
	unzClose(uf);
	return true;
#endif
}

bool Package::saveAs(const QString &path) const {
#ifdef QTDOCTEMPLATE_USE_LIBZIP
	int errp = 0;
	zip_t *archive = zip_open(path.toUtf8().constData(), ZIP_TRUNCATE | ZIP_CREATE, &errp);
	if(!archive) {
		qWarning() << "libzip: cannot create" << path << "err" << errp;
		return false;
	}
	for(auto it = m_parts.constBegin(); it != m_parts.constEnd(); ++it) {
		QByteArray nameUtf8 = it.key().toUtf8();
		zip_source_t *src = zip_source_buffer(archive, it.value().constData(), it.value().size(), 0);
		if(!src) { qWarning() << "libzip: source_buffer failed for" << it.key(); continue; }
		if(zip_file_add(archive, nameUtf8.constData(), src, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8) < 0) {
			qWarning() << "libzip: file_add failed for" << it.key();
			zip_source_free(src);
		}
	}
	if(zip_close(archive) != 0) {
		qWarning() << "libzip: close failed";
		return false;
	}
	return true;
#else
	zipFile zf = zipOpen(path.toUtf8().constData(), APPEND_STATUS_CREATE);
	if(!zf) return false;
	for(auto it = m_parts.constBegin(); it != m_parts.constEnd(); ++it) {
		QByteArray nameUtf8 = it.key().toUtf8();
		zip_fileinfo zi{};
		if(zipOpenNewFileInZip(zf, nameUtf8.constData(), &zi,
								nullptr,0,nullptr,0,nullptr,
								Z_DEFLATED, Z_DEFAULT_COMPRESSION) != ZIP_OK) {
			qWarning() << "minizip: open new file failed for" << it.key();
			continue;
		}
		if(zipWriteInFileInZip(zf, it.value().constData(), it.value().size()) != ZIP_OK) {
			qWarning() << "minizip: write failed for" << it.key();
		}
		zipCloseFileInZip(zf);
	}
	zipClose(zf, nullptr);
	return true;
#endif
}

std::optional<QByteArray> Package::readPart(const QString &name) const {
	QString key = name; const_cast<Package*>(this)->normalizePath(key);
	auto it = m_parts.find(key);
	if(it == m_parts.end()) return std::nullopt;
	return it.value();
}

void Package::writePart(const QString &name, const QByteArray &data) {
	QString key = name; normalizePath(key);
	m_parts.insert(key, data);
}

int Package::nextImageIndex(const QString &ext) const {
	QRegularExpression re(QStringLiteral(R"(word/media/image(\d+)\.)") );
	int maxIdx = 0;
	for(const QString &k : m_parts.keys()) {
		if(k.startsWith("word/media/image") && k.endsWith('.'+ext)) {
			auto m = re.match(k);
			if(m.hasMatch()) {
				int idx = m.captured(1).toInt();
				if(idx > maxIdx) maxIdx = idx;
			}
		}
	}
	return maxIdx + 1;
}

void Package::ensureDefaultContentType(const QString &ext, const QString &mime) {
	const QString ctName = QStringLiteral("[Content_Types].xml");
	QByteArray xml;
	if(auto existing = readPart(ctName)) {
		xml = *existing;
	} else {
		xml = QByteArray("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
						 "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">\n"
						 "  <Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>\n"
						 "  <Default Extension=\"xml\"  ContentType=\"application/xml\"/>\n"
						 "</Types>");
	}
	QString tag = QString("<Default Extension=\"%1\" ContentType=\"%2\"/>").arg(ext, mime);
	if(!xml.contains(tag.toUtf8())) {
		// naive insertion before closing </Types>
		int pos = xml.lastIndexOf("</Types>");
		if(pos >= 0) {
			QByteArray insertion = QByteArray("  ") + tag.toUtf8() + '\n';
			xml.insert(pos, insertion);
		}
		writePart(ctName, xml);
	}
}

QString Package::addMedia(const QByteArray &bytes, const QString &extIn) {
	QString ext = extIn.toLower();
	if(ext.startsWith('.')) ext.remove(0,1);
	int idx = nextImageIndex(ext);
	QString partName = QStringLiteral("word/media/image%1.%2").arg(idx).arg(ext);
	writePart(partName, bytes);
	// register content type (subset needed for parity)
	if(ext == "png") ensureDefaultContentType(ext, "image/png");
	else if(ext == "jpg" || ext == "jpeg") ensureDefaultContentType(ext, "image/jpeg");
	else if(ext == "gif") ensureDefaultContentType(ext, "image/gif");
	return partName;
}

}} // namespace QtDocxTemplate::opc
