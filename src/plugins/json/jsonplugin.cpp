/*
 * JSON Tiled Plugin
 * Copyright 2011, Batiste Bieler <batiste.bieler@gmail.com>
 * Based on the Lua plugin from Thorbjørn Lindeijer
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

#include "jsonplugin.h"

#include "map.h"
#include "mapobject.h"
#include "objectgroup.h"
#include "properties.h"
#include "tile.h"
#include "tilelayer.h"
#include "tileset.h"

#include <qjson/parser.h>
#include <qjson/serializer.h>
#include <QVariant>
 #include <QDebug>

#include <QFile>


using namespace JSON;
using namespace Tiled;

JSONPlugin::JSONPlugin()
{
    
}

bool JSONPlugin::write(const Map *map, const QString &fileName)
{
    file = new QFile(fileName);

    if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
        mError = tr("Could not open file for writing.");
        return false;
    }

    mMapDir = QFileInfo(fileName).path();

    QVariantMap json;
    
    json.insert("version", "1.1");
    
    const char *orientation = "unknown";
    switch (map->orientation()) {
    case Map::Unknown:
        break;
    case Map::Orthogonal:
        orientation = "orthogonal";
        break;
    case Map::Isometric:
        orientation = "isometric";
        break;
    case Map::Hexagonal:
        orientation = "hexagonal";
        break;
    }
    
    json.insert("orientation", orientation);
    json.insert("width", map->width());
    json.insert("height", map->height());
    json.insert("tilewidth", map->tileWidth());
    json.insert("tileheight", map->tileHeight());
    
    QVariantList tilesets;
    
    uint firstGid = 1;
    mGidMapper.clear();
    
    foreach (Tileset *tileset, map->tilesets()) {
        QVariantMap t;
        t.insert("name", tileset->name());
        t.insert("firstgid", firstGid);
        t.insert("gid", firstGid);
        createTileset(tileset, &t);
        tilesets.append(t);
        
        mGidMapper.insert(firstGid, tileset);
        
        firstGid += tileset->tileCount();
    }
    
    json.insert("tilesets", tilesets);
    
    QVariantList layers;
    foreach (Layer *layer, map->layers()) {
        QVariantMap l;
        if (TileLayer *tileLayer = layer->asTileLayer()) {
            createLayer(tileLayer, &l);
            layers.append(l);
        }
        else if (ObjectGroup *objectGroup = layer->asObjectGroup()) {
           createObjectGroup(objectGroup, &l);
            layers.append(l);
        }
        
    }

    json.insert("layers", layers);
    
    QJson::Serializer serializer;
    QByteArray json_string = serializer.serialize(json);
    file->write(json_string);
    file->close();

    return true;
}

void JSONPlugin::createTileset(const Tileset *tileset,
                             QVariantMap * target)
{

    if (!tileset->fileName().isEmpty()) {
        const QString rel = mMapDir.relativeFilePath(tileset->fileName());
        target->insert("filename", rel);
    } else {
        target->insert("tilewidth", tileset->tileWidth());
        target->insert("tileheight", tileset->tileHeight());
        target->insert("spacing", tileset->tileSpacing());
        target->insert("margin", tileset->margin());

        const QString rel = mMapDir.relativeFilePath(tileset->imageSource());
        target->insert("image", rel);
        target->insert("imagewidth", tileset->imageWidth());
        target->insert("imageheight", tileset->imageHeight());

        if (tileset->transparentColor().isValid()) {
            target->insert("transparentColor",
                                    tileset->transparentColor().name());
        }
    }

    QVariantList tiles;
    for (int i = 0; i < tileset->tileCount(); ++i) {
        const Tile *tile = tileset->tileAt(i);
        const Properties properties = tile->properties();
        // Include enties for those tiles that have properties set on them
        if (!properties.isEmpty()) {
            QVariantMap p;
            p.insert("id", i);
            createProperties(&p, tile->properties());
            tiles.append(p);
        }
    }
    target->insert("tiles", tiles);
    
}


void JSONPlugin::createLayer(const TileLayer *tileLayer,
                             QVariantMap * target) {

    target->insert("type", "tilelayer");
    target->insert("name", tileLayer->name());
    target->insert("x", tileLayer->x());
    target->insert("y", tileLayer->y());
    target->insert("width", tileLayer->width());
    target->insert("height", tileLayer->height());
    target->insert("visible", tileLayer->isVisible());
    target->insert("opacity", tileLayer->opacity());
    createProperties(target, tileLayer->properties());

    target->insert("encoding", "json");
    
    QVariantMap grid;
    for (int y = 0; y < tileLayer->height(); ++y) {
        // I choosed the format cX;Y
        for (int x = 0; x < tileLayer->width(); ++x) {
            QString key = QString("c%1;%2").arg(x).arg(y);
            grid.insert(key, mGidMapper.cellToGid(tileLayer->cellAt(x, y)));
        }
    }
    target->insert("grid", grid);
}

void JSONPlugin::createObjectGroup(const ObjectGroup *objectGroup,  
                                                QVariantMap * target)
{

    target->insert("type", "objectgroup");
    target->insert("name", objectGroup->name());
    target->insert("visible", objectGroup->isVisible());
    target->insert("opacity", objectGroup->opacity());
    createProperties(target, objectGroup->properties());

    QVariantList objects;
    foreach (MapObject *mapObject, objectGroup->objects()) {
        QVariantMap target;
        createMapObject(mapObject, &target);
        objects.append(target);
    }
    target->insert("objects", objects);
}

void JSONPlugin::createProperties(QVariantMap *p,
                                const Properties &properties)
{
    Properties::const_iterator it = properties.constBegin();
    Properties::const_iterator it_end = properties.constEnd();
    for (; it != it_end; ++it)
        p->insert(it.key(), it.value());

}

void JSONPlugin::createMapObject(const Tiled::MapObject *mapObject, QVariantMap * target)
{

    target->insert("name", mapObject->name());
    target->insert("type", mapObject->type());
    
    // incorrect values regarding to the isometric projection
    target->insert("x",mapObject->x());
    target->insert("y", mapObject->y());
    target->insert("width", mapObject->width());
    target->insert("height", mapObject->height());

    if (Tile *tile = mapObject->tile())
        target->insert("gid", mGidMapper.cellToGid(Cell(tile)));

    // TODO: ploygon
}

QString JSONPlugin::nameFilter() const
{
    return tr("JSON files (*.json)");
}

QString JSONPlugin::errorString() const
{
    return mError;
}


Q_EXPORT_PLUGIN2(JSON, JSONPlugin)
