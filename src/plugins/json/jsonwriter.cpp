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

#include "jsontablewriter.h"

#include <QIODevice>

namespace JSON {

JSONTableWriter::JSONTableWriter(QIODevice *device)
    : m_device(device)
    , m_indent(0)
    , m_valueSeparator(',')
    , m_newLine(true)
    , m_valueWritten(false)
    , m_error(false)
{
}

void JSONTableWriter::writeStartDocument()
{
    Q_ASSERT(m_indent == 0);
}

void JSONTableWriter::writeEndDocument()
{
    Q_ASSERT(m_indent == 0);
    write('\n');
}

void JSONTableWriter::writeStartTable()
{
    prepareNewLine();
    write('{');
    ++m_indent;
    m_newLine = false;
    m_valueWritten = false;
}

void JSONTableWriter::writeStartReturnTable()
{
    prepareNewLine();
    write("return {");
    ++m_indent;
    m_newLine = false;
    m_valueWritten = false;
}

void JSONTableWriter::writeStartTable(const QByteArray &name)
{
    prepareNewLine();
    write(name + " = {");
    ++m_indent;
    m_newLine = false;
    m_valueWritten = false;
}

void JSONTableWriter::writeEndTable()
{
    --m_indent;
    if (m_valueWritten)
        writeNewline();
    write('}');
    m_newLine = false;
    m_valueWritten = true;
}

void JSONTableWriter::writeValue(const QByteArray &value)
{
    prepareNewValue();
    write('"');
    write(value);
    write('"');
    m_newLine = false;
    m_valueWritten = true;
}

void JSONTableWriter::writeUnquotedValue(const QByteArray &value)
{
    prepareNewValue();
    write(value);
    m_newLine = false;
    m_valueWritten = true;
}

void JSONTableWriter::writeKeyAndValue(const QByteArray &key,
                                      const char *value)
{
    prepareNewLine();
    write(key);
    write(" = \"");
    write(value);
    write('"');
    m_newLine = false;
    m_valueWritten = true;
}

void JSONTableWriter::writeKeyAndValue(const QByteArray &key,
                                      const QByteArray &value)
{
    prepareNewLine();
    write(key);
    write(" = \"");
    write(value);
    write('"');
    m_newLine = false;
    m_valueWritten = true;
}

void JSONTableWriter::writeQuotedKeyAndValue(const QString &key,
                                            const QString &value)
{
    prepareNewLine();
    write("[\"");
    write(key.toUtf8());
    write("\"] = \"");
    write(value.toUtf8());
    write('"');
    m_newLine = false;
    m_valueWritten = true;
}

void JSONTableWriter::writeKeyAndUnquotedValue(const QByteArray &key,
                                              const QByteArray &value)
{
    prepareNewLine();
    write(key);
    write(" = ");
    write(value);
    m_newLine = false;
    m_valueWritten = true;
}

void JSONTableWriter::prepareNewLine()
{
    if (m_valueWritten) {
        write(m_valueSeparator);
        m_valueWritten = false;
    }
    writeNewline();
}

void JSONTableWriter::prepareNewValue()
{
    if (!m_valueWritten) {
        writeNewline();
    } else {
        write(m_valueSeparator);
        write(' ');
    }
}

void JSONTableWriter::writeIndent()
{
    for (int level = m_indent; level; --level)
        write("  ");
}

void JSONTableWriter::writeNewline()
{
    if (!m_newLine) {
        if (m_suppressNewlines) {
            write(' ');
        } else {
            write('\n');
            writeIndent();
        }
        m_newLine = true;
    }
}

void JSONTableWriter::write(const char *bytes, uint length)
{
    if (m_device->write(bytes, length) != length)
        m_error = true;
}

} // namespace JSON
