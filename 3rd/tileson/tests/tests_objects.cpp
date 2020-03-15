//
// Created by robin on 31.07.2019.
//

#include "../external_libs/catch.hpp"
#include "../src/objects/Color.hpp"

TEST_CASE( "Parse a color from Tiled color string with alpha - expect valid color", "[color][argb]" ) {
    tson::Colori color {"#ffaa07ff"};
    tson::Colori expected {170, 7, 255, 255 };
    REQUIRE( color == expected );
}

TEST_CASE( "Parse a FLOAT color from Tiled color string with alpha - expect valid color", "[color][argb]" ) {
    tson::Colorf color {"#ffaa07ff"};
    tson::Colorf expected = tson::Colori(170, 7, 255, 255).asFloat();
    REQUIRE( color == expected );
}

TEST_CASE( "Parse a color from Tiled color string without alpha - expect valid color", "[color][rgb]" ) {
    tson::Colori color {"#aa07ff"};
    tson::Colori expected {170, 7, 255, 255 };
    REQUIRE( color == expected );
}

TEST_CASE( "Parse a color from Tiled color and get as float- expect correct float values", "[color][float]" ) {
    tson::Colori color {29, 170, 205, 255};
    tson::Colorf expected {(float)29 / 255, (float)170 / 255, (float)205 / 255, (float)255 / 255};
    tson::Colorf result = color.asFloat();
    REQUIRE( result == expected );
}

TEST_CASE( "Compare color and its related hex as sting - expect success", "[color][comparison][string]" ) {
    tson::Colori color {170, 7, 255, 255 };
    REQUIRE( color == "#ffaa07ff" );
}