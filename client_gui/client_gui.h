#pragma once

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Shaders/PhongGL.h>
#include <Magnum/Trade/MeshData.h>
using namespace Magnum;

namespace zs
{
    class App
    {
    public:
        App(const Vector2i& windowSize);
        void Tick();

    private:
        GL::Mesh _mesh;
        Shaders::PhongGL _shader;

        Matrix4 _transformation, _projection;
        Color3 _color;
    };
}