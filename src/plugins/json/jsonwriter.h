/*
 * JSON Tiled Plugin
 * Copyright 2011, Thorbjørn Lindeijer <thorbjorn@lindeijer.nl>
 *
 * This file is part of Tiled.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LUATABLEWRITER_H
#define LUATABLEWRITER_H

#include <QByteArray>
#include <QString>

class QIODevice;

namespace JSON {

/**
 * Makes it easy to produce a well formatted JSON structure.
 */
class JSONTableWriter
{
public:
    JSONTableWriter(QIODevice *device);

    void writeStartDocument();
    void writeEndDocument();

    void writeStartTable();
    void writeStartReturnTable();
    void writeStartTable(const QByteArray &name);
    void writeEndTable();

    void writeValue(int value);
    void writeValue(uint value);
    void writeValue(const QByteArray &value);
    void writeValue(const QString &value);

    void writeUnquotedValue(const QByteArray &value);

    void writeKeyAndValue(const QByteArray &key, int value);
    void writeKeyAndValue(const QByteArray &key, uint value);
    void writeKeyAndValue(const QByteArray &key, double value);
    void writeKeyAndValue(const QByteArray &key, bool value);
    void writeKeyAndValue(const QByteArray &key, const char *value);
    void writeKeyAndValue(const QByteArray &key, const QByteArray &value);
    void writeKeyAndValue(const QByteArray &key, const QString &value);

    void writeQuotedKeyAndValue(const QString &key, const QString &value);
    void writeKeyAndUnquotedValue(const QByteArray &key,
                                  const QByteArray &value);

    void setSuppressNewlines(bool suppressNewlines);
    bool suppressNewlines() const;

    void prepareNewLine();

    bool hasError() const { return m_error; }

private:
    void prepareNewValue();
    void writeIndent();

    void writeNewline();
    void write(const char *bytes, uint length);
    void write(const char *bytes);
    void write(const QByteArray &bytes);
    void write(char c);

    QIODevice *m_device;
    int m_indent;
    char m_valueSeparator;
    bool m_suppressNewlines;
    bool m_newLine;
    bool m_valueWritten;
    bool m_error;
};

inline void JSONTableWriter::writeValue(int value)
{ writeUnquotedValue(QByteArray::number(value)); }

inline void JSONTableWriter::writeValue(uint value)
{ writeUnquotedValue(QByteArray::number(value)); }

inline void JSONTableWriter::writeValue(const QString &value)
{ writeValue(value.toUtf8()); }

inline void JSONTableWriter::writeKeyAndValue(const QByteArray &key, int value)
{ writeKeyAndUnquotedValue(key, QByteArray::number(value)); }

inline void JSONTableWriter::writeKeyAndValue(const QByteArray &key, uint value)
{ writeKeyAndUnquotedValue(key, QByteArray::number(value)); }

inline void JSONTableWriter::writeKeyAndValue(const QByteArray &key, double value)
{ writeKeyAndUnquotedValue(key, QByteArray::number(value)); }

inline void JSONTableWriter::writeKeyAndValue(const QByteArray &key, bool value)
{ writeKeyAndUnquotedValue(key, value ? "true" : "false"); }

inline void JSONTableWriter::writeKeyAndValue(const QByteArray &key, const QString &value)
{ writeKeyAndValue(key, value.toUtf8()); }

inline void JSONTableWriter::write(const char *bytes)
{ write(bytes, qstrlen(bytes)); }

inline void JSONTableWriter::write(const QByteArray &bytes)
{ write(bytes.constData(), bytes.length()); }

inline void JSONTableWriter::write(char c)
{ write(&c, 1); }

/**
 * Sets whether newlines should be suppressed. While newlines are suppressed,
 * the writer will write out spaces instead of newlines.
 */
inline void JSONTableWriter::setSuppressNewlines(bool suppressNewlines)
{ m_suppressNewlines = suppressNewlines; }

inline bool JSONTableWriter::suppressNewlines() const
{ return m_suppressNewlines; }

} // namespace JSON

#endif // LUATABLEWRITER_H
