#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <Eigen/Eigen>
#include "../common/ComponentTraits.h"
#include "../common/Archive.h"

struct Name
{
    char name[16];
};

struct TestData
{
    float speed;
    double scale;
    Eigen::Vector3f velocity;
    bool alive;
    Name name;
};

TEST_CASE("save & load")
{
    TestData origin;
    origin.speed=10.f;
    origin.scale=3.3f;
    origin.velocity={0.13f,1.2f,-9.8f};
    origin.alive=true;
    origin.name=Name{'e','v','a','\0'};

    ct::OutputStringArchive out;
    ct::BitwiseComponentTraits<TestData>::Archive(out,origin);

    ct::InputStringArchive in(out.String());
    TestData loaded;
    ct::BitwiseComponentTraits<TestData>::Load(in,loaded);

    REQUIRE(loaded.speed==10.f);
    REQUIRE(loaded.scale==3.3f);
    REQUIRE(loaded.velocity==Eigen::Vector3f{0.13f,1.2f,-9.8f});
    REQUIRE(loaded.alive==true);
    REQUIRE(loaded.name.name[0]=='e');
    REQUIRE(loaded.name.name[1]=='v');
    REQUIRE(loaded.name.name[2]=='a');
    REQUIRE(loaded.name.name[3]=='\0');
}