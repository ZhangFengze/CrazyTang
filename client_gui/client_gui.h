#pragma once

#include <memory>

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

#include "../common/Net.h"
#include "../common/Entity.h"
#include "../common/Voxel.h"

using namespace Magnum;

namespace zs
{
    class App
    {
    public:
        App(const Vector2i& windowSize);
        void Tick();
        void OnMouseMove(float dx, float dy);

    private:
        void Draw();
        void TickImGui();

        asio::awaitable<void> Login(asio::io_context& io);

    private:
        GL::Mesh _mesh;
        Shaders::PhongGL _shader;

        Matrix4 _transformation, _projection;
        Color3 _color;

        Vector2 windowSize_;
        Vector3 boxPos{ 0,0,0 };
        Vector3 cameraPos{ 0,0,0 };
        float cameraYaw = 0.f;
        float cameraPitch = 0.f;

    private:
        asio::io_context io;

        std::shared_ptr<ct::NetAgent> curAgent;
        uint64_t curID = 0;
        ct::EntityContainer curEntities;
        ct::voxel::Container curVoxels;
    };
}