#pragma once

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Shaders/VertexColorGL.h>

using namespace Magnum;

namespace zs
{
    class App
    {
    public:
        App();
        void Tick();

        GL::Mesh _mesh;
        Shaders::VertexColorGL2D _shader;
    };
}