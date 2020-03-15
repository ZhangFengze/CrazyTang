//
// Created by robin on 31.07.2019.
//

#include <tiled/Map.h>
#include <tiled/Layer.h>
#include <tiled/Chunk.h>
#include <iostream>
#include <tiled/Object.h>
#include <tiled/Tileset.h>
#include <tiled/Tile.h>
#include <tiled/Frame.h>
#include <tiled/Terrain.h>
#include <tiled/WangColor.h>
#include <tiled/WangTile.h>
#include <tiled/WangSet.h>
#include "../external_libs/catch.hpp"

TEST_CASE( "Parse a Map from Tiled's documentation", "[tiled][map]" )
{

    nlohmann::json j = "{\n"
                       "  \"backgroundcolor\":\"#656667\",\n"
                       "  \"height\":4,\n"
                       "  \"layers\":[ ],\n"
                       "  \"nextobjectid\":1,\n"
                       "  \"orientation\":\"orthogonal\",\n"
                       "  \"properties\":[\n"
                       "    {\n"
                       "      \"name\":\"mapProperty1\",\n"
                       "      \"type\":\"one\",\n"
                       "      \"value\":\"string\"\n"
                       "    },\n"
                       "    {\n"
                       "      \"name\":\"mapProperty2\",\n"
                       "      \"type\":\"two\",\n"
                       "      \"value\":\"string\"\n"
                       "    }],\n"
                       "  \"renderorder\":\"right-down\",\n"
                       "  \"tileheight\":32,\n"
                       "  \"tilesets\":[ ],\n"
                       "  \"tilewidth\":32,\n"
                       "  \"version\":1,\n"
                       "  \"tiledversion\":\"1.0.3\",\n"
                       "  \"width\":4\n"
                       "}"_json;

    tson::Map map;
    bool parseOk = map.parse(j);
    bool hasCorrectValues = (
                                map.getBackgroundColor() == "#656667" &&
                                map.getSize() == tson::Vector2i(4,4) &&
                                map.getNextObjectId() == 1 &&
                                map.getOrientation() == "orthogonal" &&
                                map.getRenderOrder() == "right-down" &&
                                map.getTileSize() == tson::Vector2i(32, 32) &&
                                map.getVersion() == 1 &&
                                map.getTiledVersion() == "1.0.3" &&
                                map.getProperties().getSize() == 2 &&
                                map.getProperties().get()[0]->getName() == "mapProperty1" &&
                                map.getProperties().getValue<std::string>("mapProperty1") == "string" &&
                                map.getProperties().getProperty("mapProperty2")->getType() == tson::Property::Type::Undefined && //The doc got some errors
                                map.get<std::string>("mapProperty1") == "string" &&
                                map.getProp("mapProperty1")->getType() == tson::Property::Type::Undefined
                            );

    REQUIRE( (parseOk && hasCorrectValues) );
}

TEST_CASE( "Parse a Layer from Tiled's documentation - read simple values", "[tiled][layer]" )
{

    SECTION("Layer - Tile Layer format")
    {
        nlohmann::json j = "{\n"
                           "  \"data\":[1, 2, 1, 2, 3, 1, 3, 1, 2, 2, 3, 3, 4, 4, 4, 1],\n"
                           "  \"height\":4,\n"
                           "  \"name\":\"ground\",\n"
                           "  \"opacity\":1,\n"
                           "  \"properties\":[\n"
                           "    {\n"
                           "      \"name\":\"tileLayerProp\",\n"
                           "      \"type\":\"int\",\n"
                           "      \"value\":1\n"
                           "    }],\n"
                           "  \"type\":\"tilelayer\",\n"
                           "  \"visible\":true,\n"
                           "  \"width\":4,\n"
                           "  \"x\":0,\n"
                           "  \"y\":0\n"
                           "}"_json;

        tson::Layer layer;
        bool parseOk = layer.parse(j);
        bool hasCorrectValues = (
                layer.getData().size() == 16 &&
                layer.getSize() == tson::Vector2i(4, 4) &&
                layer.getName() == "ground" &&
                layer.getOpacity() == 1 &&
                        layer.getTypeStr() == "tilelayer" &&
                layer.isVisible() &&
                layer.getX() == 0 &&
                layer.getY() == 0 &&
                layer.getProperties().getSize() > 0 &&
                layer.getProperties().getValue<int>("tileLayerProp") == 1 &&
                layer.getProperties().getProperty("tileLayerProp")->getType() == tson::Property::Type::Int

        );


        REQUIRE((parseOk && hasCorrectValues));
    }

    SECTION("Layer - Object Layer format")
    {
        nlohmann::json j = "{\n"
                           "  \"draworder\":\"topdown\",\n"
                           "  \"height\":8,\n"
                           "  \"name\":\"people\",\n"
                           "  \"objects\":[ ],\n"
                           "  \"opacity\":1,\n"
                           "  \"properties\":[\n"
                           "    {\n"
                           "      \"name\":\"layerProp1\",\n"
                           "      \"type\":\"string\",\n"
                           "      \"value\":\"someStringValue\"\n"
                           "    }],\n"
                           "  \"type\":\"objectgroup\",\n"
                           "  \"visible\":true,\n"
                           "  \"width\":4,\n"
                           "  \"x\":0,\n"
                           "  \"y\":0\n"
                           "}"_json;

        tson::Layer layer;
        bool parseOk = layer.parse(j);
        bool hasCorrectValues = (
                layer.getDrawOrder() == "topdown" &&
                layer.getName() == "people" &&
                layer.getOpacity() == 1 &&
                        layer.getTypeStr() == "objectgroup" &&
                layer.isVisible() &&
                layer.getSize() == tson::Vector2i(4, 8) &&
                layer.getX() == 0 &&
                layer.getY() == 0 &&
                layer.getProperties().getValue<std::string>("layerProp1") == "someStringValue" &&
                layer.getProperties().getProperty("layerProp1")->getType() == tson::Property::Type::String &&
                layer.getObjects().empty()
        );


        REQUIRE((parseOk && hasCorrectValues));
    }
}

TEST_CASE( "Parse a Chunk from Tiled's documentation - read simple values", "[tiled][chunk]" )
{
    nlohmann::json j = "{\n"
                       "  \"data\":[1, 2, 1, 2, 3, 1, 3, 1, 2, 2, 3, 3, 4, 4, 4, 1],\n"
                       "  \"height\":16,\n"
                       "  \"width\":16,\n"
                       "  \"x\":4,\n"
                       "  \"y\":-16\n"
                       "}"_json;

    tson::Chunk chunk;
    bool parseOk = chunk.parse(j);
    bool hasCorrectValues = (
            chunk.getData().size() == 16 &&
            chunk.getSize() == tson::Vector2i(16, 16) &&
            chunk.getPosition() == tson::Vector2i(4, -16)
    );

    REQUIRE((parseOk && hasCorrectValues));
}

TEST_CASE( "Parse an Object from Tiled's documentation - read simple values", "[tiled][object]" )
{
    SECTION("Object - regular")
    {
        nlohmann::json j = "{\n"
                           "  \"gid\":5,\n"
                           "  \"height\":0,\n"
                           "  \"id\":1,\n"
                           "  \"name\":\"villager\",\n"
                           "  \"properties\":[\n"
                           "    {\n"
                           "      \"name\":\"hp\",\n"
                           "      \"type\":\"int\",\n"
                           "      \"value\":12\n"
                           "    }],\n"
                           "  \"rotation\":90,\n"
                           "  \"type\":\"npc\",\n"
                           "  \"visible\":true,\n"
                           "  \"width\":0,\n"
                           "  \"x\":32,\n"
                           "  \"y\":32\n"
                           "}"_json;

        tson::Object obj;
        bool parseOk = obj.parse(j);
        bool hasCorrectValues = (
                obj.getGid() == 5 &&
                obj.getId() == 1 &&
                obj.getName() == "villager" &&
                obj.getRotation() == 90 &&
                obj.getType() == "npc" &&
                obj.isVisible() &&
                obj.getSize() == tson::Vector2i(0, 0) &&
                obj.getPosition() == tson::Vector2i(32, 32) &&
                obj.getObjectType() == tson::Object::Type::Object &&
                obj.getProperties().getValue<int>("hp") == 12 &&
                obj.getProperties().getProperty("hp")->getType() == tson::Property::Type::Int &&
                obj.get<int>("hp") == 12 &&
                obj.getProp("hp")->getType() == tson::Property::Type::Int
        );

        REQUIRE((parseOk && hasCorrectValues));
    }

    SECTION("Object - ellipse")
    {
        nlohmann::json j = "{\n"
                           "  \"ellipse\":true,\n"
                           "  \"height\":152,\n"
                           "  \"id\":13,\n"
                           "  \"name\":\"\",\n"
                           "  \"rotation\":0,\n"
                           "  \"type\":\"\",\n"
                           "  \"visible\":true,\n"
                           "  \"width\":248,\n"
                           "  \"x\":560,\n"
                           "  \"y\":808\n"
                           "}"_json;

        tson::Object obj;
        bool parseOk = obj.parse(j);
        bool hasCorrectValues = (
                obj.isEllipse() &&
                obj.getId() == 13 &&
                obj.getName().empty() &&
                obj.getRotation() == 0 &&
                obj.getType().empty() &&
                obj.isVisible() &&
                obj.getSize() == tson::Vector2i(248, 152) &&
                obj.getPosition() == tson::Vector2i(560, 808)
                && obj.getObjectType() == tson::Object::Type::Ellipse
        );

        REQUIRE((parseOk && hasCorrectValues));
    }

    SECTION("Object - rectangle")
    {
        nlohmann::json j = "{\n"
                           "  \"height\":184,\n"
                           "  \"id\":14,\n"
                           "  \"name\":\"\",\n"
                           "  \"rotation\":0,\n"
                           "  \"type\":\"\",\n"
                           "  \"visible\":true,\n"
                           "  \"width\":368,\n"
                           "  \"x\":576,\n"
                           "  \"y\":584\n"
                           "}"_json;

        tson::Object obj;
        bool parseOk = obj.parse(j);
        bool hasCorrectValues = (
                obj.getId() == 14 &&
                obj.getName().empty() &&
                obj.getRotation() == 0 &&
                obj.getType().empty() &&
                obj.isVisible() &&
                obj.getSize() == tson::Vector2i(368, 184) &&
                obj.getPosition() == tson::Vector2i(576, 584)
                && obj.getObjectType() == tson::Object::Type::Rectangle
        );

        REQUIRE((parseOk && hasCorrectValues));
    }

    SECTION("Object - point")
    {
        nlohmann::json j = "{\n"
                           "  \"point\":true,\n"
                           "  \"height\":0,\n"
                           "  \"id\":20,\n"
                           "  \"name\":\"\",\n"
                           "  \"rotation\":0,\n"
                           "  \"type\":\"\",\n"
                           "  \"visible\":true,\n"
                           "  \"width\":0,\n"
                           "  \"x\":220,\n"
                           "  \"y\":350\n"
                           "}"_json;

        tson::Object obj;
        bool parseOk = obj.parse(j);
        bool hasCorrectValues = (
                obj.isPoint() &&
                obj.getId() == 20 &&
                obj.getName().empty() &&
                obj.getRotation() == 0 &&
                obj.getType().empty() &&
                obj.isVisible() &&
                obj.getSize() == tson::Vector2i(0, 0) &&
                obj.getPosition() == tson::Vector2i(220, 350)
                && obj.getObjectType() == tson::Object::Type::Point
        );

        REQUIRE((parseOk && hasCorrectValues));
    }

    SECTION("Object - polygon")
    {
        nlohmann::json j = "{\n"
                           "  \"height\":0,\n"
                           "  \"id\":15,\n"
                           "  \"name\":\"\",\n"
                           "  \"polygon\":[\n"
                           "  {\n"
                           "    \"x\":0,\n"
                           "    \"y\":0\n"
                           "  },\n"
                           "  {\n"
                           "    \"x\":152,\n"
                           "    \"y\":88\n"
                           "  },\n"
                           "  {\n"
                           "    \"x\":136,\n"
                           "    \"y\":-128\n"
                           "  },\n"
                           "  {\n"
                           "    \"x\":80,\n"
                           "    \"y\":-280\n"
                           "  },\n"
                           "  {\n"
                           "    \"x\":16,\n"
                           "    \"y\":-288\n"
                           "  }],\n"
                           "  \"rotation\":0,\n"
                           "  \"type\":\"\",\n"
                           "  \"visible\":true,\n"
                           "  \"width\":0,\n"
                           "  \"x\":-176,\n"
                           "  \"y\":432\n"
                           "}"_json;

        tson::Object obj;
        bool parseOk = obj.parse(j);
        bool hasCorrectValues = (
                obj.getId() == 15 &&
                obj.getName().empty() &&
                obj.getRotation() == 0 &&
                obj.getType().empty() &&
                obj.isVisible() &&
                obj.getSize() == tson::Vector2i(0, 0) &&
                obj.getPosition() == tson::Vector2i(-176, 432) &&
                obj.getObjectType() == tson::Object::Type::Polygon &&
                obj.getPolygons().size() == 5 &&
                obj.getPolygons()[2] == tson::Vector2i(136, -128)
        );
    }
    SECTION("Object - polyline")
    {
        nlohmann::json j = "{\n"
                           "  \"height\":0,\n"
                           "  \"id\":16,\n"
                           "  \"name\":\"\",\n"
                           "  \"polyline\":[\n"
                           "  {\n"
                           "    \"x\":0,\n"
                           "    \"y\":0\n"
                           "  },\n"
                           "  {\n"
                           "    \"x\":248,\n"
                           "    \"y\":-32\n"
                           "  },\n"
                           "  {\n"
                           "    \"x\":376,\n"
                           "    \"y\":72\n"
                           "  },\n"
                           "  {\n"
                           "    \"x\":544,\n"
                           "    \"y\":288\n"
                           "  },\n"
                           "  {\n"
                           "    \"x\":656,\n"
                           "    \"y\":120\n"
                           "  },\n"
                           "  {\n"
                           "    \"x\":512,\n"
                           "    \"y\":0\n"
                           "  }],\n"
                           "  \"rotation\":0,\n"
                           "  \"type\":\"\",\n"
                           "  \"visible\":true,\n"
                           "  \"width\":0,\n"
                           "  \"x\":240,\n"
                           "  \"y\":88\n"
                           "}"_json;

        tson::Object obj;
        bool parseOk = obj.parse(j);
        bool hasCorrectValues = (
                obj.getId() == 16 &&
                obj.getName().empty() &&
                obj.getRotation() == 0 &&
                obj.getType().empty() &&
                obj.isVisible() &&
                obj.getSize() == tson::Vector2i(0, 0) &&
                obj.getPosition() == tson::Vector2i(240, 88) &&
                obj.getObjectType() == tson::Object::Type::Polyline &&
                obj.getPolylines().size() == 6 &&
                obj.getPolylines()[4] == tson::Vector2i(656, 120)
        );

        REQUIRE((parseOk && hasCorrectValues));
    }
    SECTION("Object - text")
    {
        nlohmann::json j = "{\n"
                           "  \"height\":19,\n"
                           "  \"id\":15,\n"
                           "  \"name\":\"\",\n"
                           "  \"text\":\n"
                           "  {\n"
                           "    \"text\":\"Hello World\",\n"
                           "    \"wrap\":true\n"
                           "  },\n"
                           "  \"rotation\":0,\n"
                           "  \"type\":\"\",\n"
                           "  \"visible\":true,\n"
                           "  \"width\":248,\n"
                           "  \"x\":48,\n"
                           "  \"y\":136\n"
                           "}"_json;

        tson::Object obj;
        bool parseOk = obj.parse(j);
        bool hasCorrectValues = (
                obj.getId() == 15 &&
                obj.getName().empty() &&
                obj.getRotation() == 0 &&
                obj.getType().empty() &&
                obj.isVisible() &&
                obj.getSize() == tson::Vector2i(248, 19) &&
                obj.getPosition() == tson::Vector2i(48, 136) &&
                obj.getObjectType() == tson::Object::Type::Text &&
                obj.getText().text == "Hello World" &&
                obj.getText().wrap
        );

        REQUIRE((parseOk && hasCorrectValues));
    }

    SECTION("Object - template")
    {
        nlohmann::json j = "{\n"
                           "    \"id\":13,\n"
                           "    \"template\":\"useless_template_object.tx\",\n"
                           "    \"x\":104,\n"
                           "    \"y\":34\n"
                           "}"_json;

        tson::Object obj;
        bool parseOk = obj.parse(j);
        bool hasCorrectValues = (
                obj.getId() == 13 &&
                obj.getTemplate() == "useless_template_object.tx" &&
                obj.getPosition() == tson::Vector2i(104, 34) &&
                obj.getObjectType() == tson::Object::Type::Template
        );

        REQUIRE((parseOk && hasCorrectValues));
    }
}

TEST_CASE( "Parse a Tileset from Tiled's documentation - read simple values", "[tiled][tileset]" )
{
    nlohmann::json j = "{\n"
                       " \"columns\":19,\n"
                       " \"firstgid\":1,\n"
                       " \"image\":\"..\\/image\\/fishbaddie_parts.png\",\n"
                       " \"imageheight\":480,\n"
                       " \"imagewidth\":640,\n"
                       " \"margin\":3,\n"
                       " \"name\":\"\",\n"
                       " \"properties\":[\n"
                       "   {\n"
                       "     \"name\":\"myProperty1\",\n"
                       "     \"type\":\"string\",\n"
                       "     \"value\":\"myProperty1_value\"\n"
                       "   }],\n"
                       " \"spacing\":1,\n"
                       " \"tilecount\":266,\n"
                       " \"tileheight\":32,\n"
                       " \"tilewidth\":32\n"
                       "}"_json;

    tson::Tileset tileset;
    bool parseOk = tileset.parse(j);
    bool hasCorrectValues = (
        tileset.getColumns() == 19 &&
        tileset.getFirstgid() == 1 &&
        #if USE_CPP17_FILESYSTEM
        tileset.getImagePath() == fs::path("../image/fishbaddie_parts.png") &&
        #endif
        tileset.getImageSize() == tson::Vector2i(640, 480) &&
        tileset.getMargin() == 3 &&
        tileset.getName().empty() &&
        tileset.getSpacing() == 1 &&
        tileset.getTileCount() == 266 &&
        tileset.getTileSize() == tson::Vector2i(32,32) &&
        tileset.getProperties().getValue<std::string>("myProperty1") == "myProperty1_value"
    );

    REQUIRE((parseOk && hasCorrectValues));
}

TEST_CASE( "Parse a Tile from Tiled's documentation - read simple values", "[tiled][tile]" )
{
    nlohmann::json j = "{\n"
                       "  \"id\":11,\n"
                       "  \"properties\":[\n"
                       "    {\n"
                       "      \"name\":\"myProperty2\",\n"
                       "      \"type\":\"string\",\n"
                       "      \"value\":\"myProperty2_value\"\n"
                       "    }],\n"
                       "  \"terrain\":[0, 1, 0, 1]\n"
                       "}"_json;

    tson::Tile tile;
    bool parseOk = tile.parse(j);
    bool hasCorrectValues = (
        tile.getId() == 12 &&
        tile.getTerrain().size() == 4 &&
        tile.getTerrain()[2] == 0 &&
        tile.getProperties().getValue<std::string>("myProperty2") == "myProperty2_value" &&
        tile.get<std::string>("myProperty2") == "myProperty2_value" &&
        tile.getProp("myProperty2") != nullptr &&
        tile.getProp("dummy") == nullptr
    );

    REQUIRE((parseOk && hasCorrectValues));
}

TEST_CASE( "Parse a Frame", "[tiled][frame]" )
{
    nlohmann::json j = "{\n"
                       "  \"duration\":100,\n"
                       "  \"tileid\":6\n"
                       "}"_json;

    tson::Frame frame;
    bool parseOk = frame.parse(j);
    bool hasCorrectValues = (
            frame.getDuration() == 100 &&
            frame.getTileId() == 6
    );

    REQUIRE((parseOk && hasCorrectValues));
}

TEST_CASE( "Parse a Terrain from Tiled's documentation", "[tiled][terrain]" )
{
    nlohmann::json j = "{\n"
                       "  \"name\":\"chasm\",\n"
                       "  \"tile\":12\n"
                       "}"_json;

    tson::Terrain terrain;
    bool parseOk = terrain.parse(j);
    bool hasCorrectValues = (
            terrain.getName() == "chasm" &&
            terrain.getTile() == 12
    );

    REQUIRE((parseOk && hasCorrectValues));
}

TEST_CASE( "Wang-tests - everything Wang - simple", "[tiled][wang]" )
{
    SECTION("WangSet")
    {
        nlohmann::json j = "{\n"
                           "    \"cornercolors\":[],\n"
                           "    \"edgecolors\":[\n"
                           "        {\n"
                           "            \"color\":\"#ff0000\",\n"
                           "            \"name\":\"Red\",\n"
                           "            \"probability\":1,\n"
                           "            \"tile\":-1\n"
                           "        }, \n"
                           "        {\n"
                           "            \"color\":\"#00ff00\",\n"
                           "            \"name\":\"Green\",\n"
                           "            \"probability\":1,\n"
                           "            \"tile\":-1\n"
                           "        }, \n"
                           "        {\n"
                           "            \"color\":\"#0000ff\",\n"
                           "            \"name\":\"Blue\",\n"
                           "            \"probability\":1,\n"
                           "            \"tile\":-1\n"
                           "        }, \n"
                           "        {\n"
                           "            \"color\":\"#ff7700\",\n"
                           "            \"name\":\"Orange\",\n"
                           "            \"probability\":1,\n"
                           "            \"tile\":-1\n"
                           "        }],\n"
                           "    \"name\":\"FirstWang\",\n"
                           "    \"properties\":[\n"
                           "        {\n"
                           "            \"name\":\"floating_wang\",\n"
                           "            \"type\":\"float\",\n"
                           "            \"value\":14.6\n"
                           "        }, \n"
                           "        {\n"
                           "            \"name\":\"is_wang\",\n"
                           "            \"type\":\"bool\",\n"
                           "            \"value\":true\n"
                           "        }],\n"
                           "    \"tile\":-1,\n"
                           "    \"wangtiles\":[\n"
                           "        {\n"
                           "            \"dflip\":false,\n"
                           "            \"hflip\":false,\n"
                           "            \"tileid\":0,\n"
                           "            \"vflip\":false,\n"
                           "            \"wangid\":[3, 0, 1, 0, 1, 0, 3, 0]\n"
                           "        }, \n"
                           "        {\n"
                           "            \"dflip\":false,\n"
                           "            \"hflip\":false,\n"
                           "            \"tileid\":1,\n"
                           "            \"vflip\":false,\n"
                           "            \"wangid\":[1, 0, 1, 0, 1, 0, 1, 0]\n"
                           "        }, \n"
                           "        {\n"
                           "            \"dflip\":false,\n"
                           "            \"hflip\":false,\n"
                           "            \"tileid\":2,\n"
                           "            \"vflip\":false,\n"
                           "            \"wangid\":[3, 0, 3, 0, 1, 0, 1, 0]\n"
                           "        }, \n"
                           "        {\n"
                           "            \"dflip\":false,\n"
                           "            \"hflip\":false,\n"
                           "            \"tileid\":3,\n"
                           "            \"vflip\":false,\n"
                           "            \"wangid\":[3, 0, 1, 0, 1, 0, 1, 0]\n"
                           "        }, \n"
                           "        {\n"
                           "            \"dflip\":false,\n"
                           "            \"hflip\":false,\n"
                           "            \"tileid\":4,\n"
                           "            \"vflip\":false,\n"
                           "            \"wangid\":[2, 0, 2, 0, 1, 0, 1, 0]\n"
                           "        }, \n"
                           "        {\n"
                           "            \"dflip\":false,\n"
                           "            \"hflip\":false,\n"
                           "            \"tileid\":8,\n"
                           "            \"vflip\":false,\n"
                           "            \"wangid\":[1, 0, 1, 0, 3, 0, 3, 0]\n"
                           "        }, \n"
                           "        {\n"
                           "            \"dflip\":false,\n"
                           "            \"hflip\":false,\n"
                           "            \"tileid\":9,\n"
                           "            \"vflip\":false,\n"
                           "            \"wangid\":[2, 0, 1, 0, 1, 0, 1, 0]\n"
                           "        }, \n"
                           "        {\n"
                           "            \"dflip\":false,\n"
                           "            \"hflip\":false,\n"
                           "            \"tileid\":10,\n"
                           "            \"vflip\":false,\n"
                           "            \"wangid\":[1, 0, 3, 0, 3, 0, 1, 0]\n"
                           "        }, \n"
                           "        {\n"
                           "            \"dflip\":false,\n"
                           "            \"hflip\":false,\n"
                           "            \"tileid\":16,\n"
                           "            \"vflip\":false,\n"
                           "            \"wangid\":[3, 0, 3, 0, 3, 0, 3, 0]\n"
                           "        }, \n"
                           "        {\n"
                           "            \"dflip\":false,\n"
                           "            \"hflip\":false,\n"
                           "            \"tileid\":17,\n"
                           "            \"vflip\":false,\n"
                           "            \"wangid\":[3, 0, 3, 0, 3, 0, 3, 0]\n"
                           "        }, \n"
                           "        {\n"
                           "            \"dflip\":false,\n"
                           "            \"hflip\":false,\n"
                           "            \"tileid\":18,\n"
                           "            \"vflip\":false,\n"
                           "            \"wangid\":[3, 0, 3, 0, 1, 0, 1, 0]\n"
                           "        }]\n"
                           "}"_json;

        tson::WangSet wangset;
        bool parseOk = wangset.parse(j);
        bool hasCorrectValues = (
                wangset.getTile() == -1 &&
                wangset.getName() == "FirstWang" &&
                wangset.getCornerColors().empty() &&
                wangset.getEdgeColors().size() == 4 &&
                wangset.getWangTiles().size() == 11 &&
                wangset.getProperties().getValue<float>("floating_wang") == 14.6f &&
                wangset.getProperties().getValue<bool>("is_wang") &&
                wangset.get<float>("floating_wang") == 14.6f &&
                wangset.getProp("is_wang")->getValue<bool>()
        );

        REQUIRE((parseOk && hasCorrectValues));
    }

    SECTION("WangColor")
    {
        nlohmann::json j = "{\n"
                           "  \"color\": \"#d31313\",\n"
                           "  \"name\": \"Rails\",\n"
                           "  \"probability\": 1,\n"
                           "  \"tile\": 18\n"
                           "}"_json;

        tson::WangColor wangColor;
        tson::Color colorMatch = tson::Colori("#d31313");
        bool parseOk = wangColor.parse(j);
        bool hasCorrectValues = (
                wangColor.getColor() == colorMatch &&
                wangColor.getName() == "Rails" &&
                wangColor.getProbability() == 1 &&
                wangColor.getTile() == 18
        );

        REQUIRE((parseOk && hasCorrectValues));
    }

    SECTION("WangTile")
    {
        nlohmann::json j = "{\n"
                           "  \"dflip\": false,\n"
                           "  \"hflip\": false,\n"
                           "  \"tileid\": 0,\n"
                           "  \"vflip\": false,\n"
                           "  \"wangid\": [2, 0, 1, 0, 1, 0, 2, 0]\n"
                           "}"_json;

        tson::WangTile wangTile;
        bool parseOk = wangTile.parse(j);
        bool hasCorrectValues = (
                !wangTile.hasDFlip() &&
                !wangTile.hasHFlip() &&
                !wangTile.hasVFlip() &&
                wangTile.getTileid() == 0 &&
                wangTile.getWangIds().size() == 8 &&
                wangTile.getWangIds()[6] == 2
        );

        REQUIRE((parseOk && hasCorrectValues));
    }
}

TEST_CASE( "Property-tests - Set properties from json", "[tiled][wang]" )
{
    nlohmann::json j = "[\n"
                       "{\n"
                       "    \"name\":\"color\",\n"
                       "    \"type\":\"color\",\n"
                       "    \"value\":\"#ff268176\"\n"
                       "}, \n"
                       "{\n"
                       "    \"name\":\"file_ref\",\n"
                       "    \"type\":\"file\",\n"
                       "    \"value\":\"..\\/demo-tileset.png\"\n"
                       "}, \n"
                       "{\n"
                       "    \"name\":\"hp\",\n"
                       "    \"type\":\"int\",\n"
                       "    \"value\":4\n"
                       "}, \n"
                       "{\n"
                       "    \"name\":\"is_player\",\n"
                       "    \"type\":\"bool\",\n"
                       "    \"value\":true\n"
                       "}, \n"
                       "{\n"
                       "    \"name\":\"jump_force\",\n"
                       "    \"type\":\"float\",\n"
                       "    \"value\":10\n"
                       "}, \n"
                       "{\n"
                       "    \"name\":\"name\",\n"
                       "    \"type\":\"string\",\n"
                       "    \"value\":\"Mario\"\n"
                       "}]"_json;

    tson::PropertyCollection properties;
    for(const auto &item : j)
    {
        properties.add(item);
    }

    bool hasCorrectValues = (
        properties.getProperties().size() == 6 &&
        properties.getValue<tson::Colori>("color") == tson::Colori("#ff268176") &&
        properties.getProperty("color")->getType() == tson::Property::Type::Color &&
        #if USE_CPP17_FILESYSTEM
        properties.getValue<fs::path>("file_ref") == fs::path("../demo-tileset.png") &&
        #endif
        properties.getProperty("file_ref")->getType() == tson::Property::Type::File &&
        properties.getValue<int>("hp") == 4 && properties.getProperty("hp")->getType() == tson::Property::Type::Int &&
        properties.getValue<bool>("is_player") && properties.getProperty("is_player")->getType() == tson::Property::Type::Boolean &&
        properties.getValue<float>("jump_force") == 10 && properties.getProperty("jump_force")->getType() == tson::Property::Type::Float &&
        properties.getValue<std::string>("name") == "Mario" && properties.getProperty("name")->getType() == tson::Property::Type::String
    );

    REQUIRE(hasCorrectValues);
}