#pragma once

#include <memory>

#include <asio.hpp>

#include <PxConfig.h>
#include <PxPhysicsAPI.h>

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Shaders/PhongGL.h>
#include <Magnum/Platform/GlfwApplication.h>

#include "../common/Net.h"
#include "../common/Entity.h"
#include "../common/Voxel.h"
#include "../common/fps.h"

#include "VoxelView.h"

using namespace Magnum;

namespace ct
{
    class App
    {
    public:
        App(const Vector2i& windowSize, GLFWwindow* window);
        void Tick();
        void OnMouseMove(float dx, float dy);

    private:
        void DrawVoxels();
        void DrawPlayers();
        void TickImGui();
        void TickInput();
        void TickVoxelsView();

    private:
        asio::awaitable<void> Login(asio::io_context& io, const asio::ip::tcp::endpoint&);
        asio::awaitable<void> Sync();

    private:
        struct Server
        {
            std::string name;
            asio::ip::tcp::endpoint endpoint;
        };
        std::vector<Server> servers_;

        asio::awaitable<void> RefreshServerList(asio::io_context& io);

    private:
        GL::Mesh voxelMesh_;
        Shaders::PhongGL voxelShader_;
        GL::Buffer voxelBuffer_;

        GL::Mesh playerMesh_;
        Shaders::PhongGL playerShader_;
        GL::Buffer playerBuffer_;

        Matrix4 projection_;
        std::unordered_map<voxel::Type, Color3> palette_;

        Vector2 windowSize_;
        GLFWwindow* window_;
        Vector3 cameraPos_{ 0,0,30 };
        float cameraYaw_ = 0.f;
        float cameraPitch_ = 0.f;

    private:
        asio::io_context io_;

        std::shared_ptr<ct::NetAgent> curAgent_;
        uint64_t curID_ = 0;
        EntityContainer curEntities_;
        voxel::Container curVoxels_;
        voxel_view::Container curVoxelsView_;
        FPS fps_;
        int drawVoxels_ = 0;
    };
}