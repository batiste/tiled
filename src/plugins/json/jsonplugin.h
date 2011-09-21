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

#ifndef JSONPLUGIN_H
#define JSONPLUGIN_H

#include "gidmapper.h"
#include "mapwriterinterface.h"

#include "map.h"
#include "mapobject.h"
#include "objectgroup.h"
#include "properties.h"
#include "tile.h"
#include "tilelayer.h"
#include "tileset.h"

#include <QDir>

#include <QObject>
#include <QFile>
#include <QVariant>
 #include <QDebug>

namespace Tiled {
    class MapObject;
    class ObjectGroup;
    class Properties;
    class TileLayer;
    class Tileset;
}

namespace JSON {

class Q_DECL_EXPORT JSONPlugin : public QObject,
                                   public Tiled::MapWriterInterface
{
    Q_OBJECT
    Q_INTERFACES(Tiled::MapWriterInterface)

public:
    JSONPlugin();

    // MapWriterInterface
    bool write(const Tiled::Map *map, const QString &fileName);
    //void writeMap(const Tiled::Map *map);

    void createTileset(const Tiled::Tileset *tileset,
                             QVariantMap * target);
    
    void createProperties(QVariantMap *p, const Tiled::Properties &properties);
    
    void createLayer(const Tiled::TileLayer *tileLayer, QVariantMap * target);
    
    void createObjectGroup(const Tiled::ObjectGroup *objectGroup,  
                                                QVariantMap * target);
    
    void createMapObject(const Tiled::MapObject *mapObject, QVariantMap * target);
    //void fwrite(const char * data);
    QString nameFilter() const;
    QString errorString() const;

    
private:

    QFile * file;
    
    QString mError;
    QDir mMapDir;     // The directory in which the map is being saved
    Tiled::GidMapper mGidMapper;
};
    
} // namespace JSON

#endif // JSONPLUGIN_H
