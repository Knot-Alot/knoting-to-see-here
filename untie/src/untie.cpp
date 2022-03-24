#include "untie.h"
#include <knoting/components.h>
#include <knoting/game_object.h>
#include <knoting/instance_mesh.h>
#include <knoting/log.h>
#include <knoting/mesh.h>
#include <knoting/post_processing.h>
#include <knoting/px_variables_wrapper.h>
#include <knoting/scene.h>
#include <knoting/texture.h>
#include "knoting/Menu.h"
#include "knoting/demo_widget.h"

#include <knoting/audio_listener.h>
#include <knoting/audio_source.h>
#include <knoting/audio_subsystem.h>
#include <knoting/components.h>
#include <knoting/scene.h>
#include <knoting/skybox.h>
#include <knoting/spot_light.h>
#include <fstream>
#include <iostream>

#include <bx/timer.h>
#include <knoting/widget_subsystem.h>
#include <cereal/archives/json.hpp>
#include <cstdio>
#include <ctime>
#include <iostream>

namespace knot {

Scene scene;
Scene loadedScene;

std::optional<GameObject> cubeOne;
std::optional<GameObject> cubeTwo;
components::Particles* partSystem;
Untie::Untie() {
    Scene::set_active_scene(scene);
    log::Logger::setup();
    m_engine = std::make_unique<knot::Engine>();
    Engine::set_active_engine(*m_engine);
    {
        auto editorCamera = scene.create_game_object("camera");
        auto& cam = editorCamera.add_component<components::EditorCamera>();
        editorCamera.get_component<components::Transform>().set_position(glm::vec3(-0.0f, 50.0f, 0.0f));
        editorCamera.add_component<components::AudioListener>();
    }
    {
        auto cubeObj = scene.create_game_object("skybox");
        cubeObj.get_component<components::Transform>().set_position(glm::vec3(0, 8, 0));
        cubeObj.add_component<components::InstanceMesh>("postProcessPlane");

        auto skybox = components::SkyBox();

        skybox.set_texture_slot_path(SkyBoxTextureType::SkyBox, "skybox/cmft_skybox.hdr");
        skybox.set_texture_slot_path(SkyBoxTextureType::Irradiance, "skybox/cmtr_irradiance.hdr");
        skybox.set_texture_slot_path(SkyBoxTextureType::Radiance, "skybox/cmtr_radiance.hdr");
        skybox.set_uniform_background_type(SkyBoxTextureType::Radiance);

        cubeObj.add_component<components::SkyBox>(skybox);
    }
    {
        auto light = scene.create_game_object("light_0");
        auto& spotLight = light.add_component<components::SpotLight>();
        spotLight.set_color(vec3(1.0f, 0.7f, 0.2f));
        spotLight.set_outer_radius(30.0f);
        spotLight.set_inner_radius(0.5f);
        light.get_component<components::Transform>().set_position(glm::vec3(-10, 15, 10));
    }
    {
        auto light = scene.create_game_object("light_1");
        auto& spotLight = light.add_component<components::SpotLight>();
        spotLight.set_color(vec3(0.7f, 0.2f, 1.0f));
        spotLight.set_outer_radius(30.0f);
        spotLight.set_inner_radius(0.5f);
        light.get_component<components::Transform>().set_position(glm::vec3(10, 15, -10));
    }
    {
        auto light = scene.create_game_object("light_2");
        auto& spotLight = light.add_component<components::SpotLight>();
        spotLight.set_color(vec3(0.2f, 1.0f, 0.7f));
        spotLight.set_outer_radius(30.0f);
        spotLight.set_inner_radius(0.5f);
        light.get_component<components::Transform>().set_position(glm::vec3(-10, 15, -10));
    }
    {
        auto light = scene.create_game_object("light_3");
        auto& spotLight = light.add_component<components::SpotLight>();
        spotLight.set_color(vec3(1.0f, 0.4f, 0.2f));
        spotLight.set_outer_radius(30.0f);
        spotLight.set_inner_radius(0.5f);
        light.get_component<components::Transform>().set_position(glm::vec3(10, 15, 10));
    }

    {
        auto cubeObj = scene.create_game_object("floor");
        cubeObj.get_component<components::Transform>().set_position(glm::vec3(-0.0f, 0.0f, -0.0f));
        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(25, 1, 25));
        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 0, 0));
        cubeObj.add_component<components::InstanceMesh>("uv_cube.obj");

        auto& physics_material = cubeObj.add_component<components::PhysicsMaterial>();

        auto& shape = cubeObj.add_component<components::Shape>();
        vec3 halfsize = vec3(25.0, 1.0f, 25.0);
        shape.set_geometry(shape.create_cube_geometry(halfsize));

        auto& aggregate = cubeObj.add_component<components::Aggregate>();
        aggregate.add_aggregate("floor", 10, false);

        auto& rigidbody = cubeObj.add_component<components::RigidBody>();

        auto material = components::Material();
        material.set_texture_slot_path(TextureType::Albedo, "UV_Grid_test.png");
        material.set_texture_slot_path(TextureType::Normal, "normal_tiles_1k.png");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
        cubeObj.add_component<components::Material>(material);

        rigidbody.create_actor(false);
    }
    // PLAYER 1
    {
        auto cubeObj = scene.create_game_object("stanford_dragon_1");
        cubeObj.get_component<components::Transform>().set_position(glm::vec3(-20, 5.0f, -20));
        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(3, 3, 3));
        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 240, 0));
        cubeObj.add_component<components::InstanceMesh>("dragon.obj");

        auto& physics_material = cubeObj.add_component<components::PhysicsMaterial>();

        auto& shape = cubeObj.add_component<components::Shape>();
        vec3 halfsize = vec3(1.5f);
        shape.set_geometry(shape.create_cube_geometry(halfsize));

        auto& aggregate = cubeObj.add_component<components::Aggregate>();
        aggregate.add_aggregate("player1", 10, false);

        auto& rigidbody = cubeObj.add_component<components::RigidBody>();

        rigidbody.create_actor(true, 4.0f);

        auto& rigidController = cubeObj.add_component<components::RigidController>();
        rigidController.lockRotations(true, false, true);
        rigidController.set_linear_damping(1.0f);
        rigidController.set_angular_damping(1.0f);

        auto material = components::Material();
        material.set_texture_slot_path(TextureType::Albedo, "oldiron/OldIron01_1K_BaseColor.png");
        material.set_texture_slot_path(TextureType::Normal, "oldiron/OldIron01_1K_Normal.png");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
        cubeObj.add_component<components::Material>(material);

        auto& client = cubeObj.add_component<components::ClientPlayer>();
        client.m_clientNum = 0;
    }
    // PLAYER 2
    {
        auto cubeObj = scene.create_game_object("stanford_dragon_2");
        cubeObj.get_component<components::Transform>().set_position(glm::vec3(20, 5.0f, 20));
        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(3, 3, 3));
        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 10, 0));
        cubeObj.add_component<components::InstanceMesh>("dragon.obj");

        auto& physics_material = cubeObj.add_component<components::PhysicsMaterial>();

        auto& shape = cubeObj.add_component<components::Shape>();
        vec3 halfsize = vec3(1.5f);
        shape.set_geometry(shape.create_cube_geometry(halfsize));

        auto& aggregate = cubeObj.add_component<components::Aggregate>();
        aggregate.add_aggregate("player2", 10, false);

        auto& rigidbody = cubeObj.add_component<components::RigidBody>();

        rigidbody.create_actor(true, 4.0f);

        auto& rigidController = cubeObj.add_component<components::RigidController>();
        rigidController.lockRotations(true, false, true);
        rigidController.set_linear_damping(1.0f);
        rigidController.set_angular_damping(1.0f);

        auto material = components::Material();
        material.set_texture_slot_path(TextureType::Albedo, "oldiron/OldIron01_1K_BaseColor.png");
        material.set_texture_slot_path(TextureType::Normal, "oldiron/OldIron01_1K_Normal.png");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
        cubeObj.add_component<components::Material>(material);

        auto& source = cubeObj.add_component<components::AudioSource>("drumloop.wav", true);

        m_engine->get_audio_module().lock()->play(source);

        auto& client = cubeObj.add_component<components::ClientPlayer>();
        client.m_clientNum = (1);
    }
    // PLAYER 3
    {
        auto cubeObj = scene.create_game_object("stanford_dragon_3");
        cubeObj.get_component<components::Transform>().set_position(glm::vec3(-20, 5.0f, 20));
        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(3, 3, 3));
        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 10, 0));
        cubeObj.add_component<components::InstanceMesh>("dragon.obj");

        auto& physics_material = cubeObj.add_component<components::PhysicsMaterial>();

        auto& shape = cubeObj.add_component<components::Shape>();
        vec3 halfsize = vec3(1.5f);
        shape.set_geometry(shape.create_cube_geometry(halfsize));

        auto& aggregate = cubeObj.add_component<components::Aggregate>();
        aggregate.add_aggregate("player3", 10, false);

        auto& rigidbody = cubeObj.add_component<components::RigidBody>();

        rigidbody.create_actor(true, 4.0f);

        auto& rigidController = cubeObj.add_component<components::RigidController>();
        rigidController.lockRotations(true, false, true);
        rigidController.set_linear_damping(1.0f);
        rigidController.set_angular_damping(1.0f);

        auto material = components::Material();
        material.set_texture_slot_path(TextureType::Albedo, "oldiron/OldIron01_1K_BaseColor.png");
        material.set_texture_slot_path(TextureType::Normal, "oldiron/OldIron01_1K_Normal.png");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
        cubeObj.add_component<components::Material>(material);

        auto& client = cubeObj.add_component<components::ClientPlayer>();
        client.m_clientNum = (2);
    }
    // PLAYER 4
    {
        auto cubeObj = scene.create_game_object("stanford_dragon_4");
        cubeObj.get_component<components::Transform>().set_position(glm::vec3(20, 5.0f, -20));
        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(3, 3, 3));
        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 10, 0));
        cubeObj.add_component<components::InstanceMesh>("dragon.obj");

        auto& physics_material = cubeObj.add_component<components::PhysicsMaterial>();

        auto& shape = cubeObj.add_component<components::Shape>();
        vec3 halfsize = vec3(1.5f);
        shape.set_geometry(shape.create_cube_geometry(halfsize));

        auto& aggregate = cubeObj.add_component<components::Aggregate>();
        aggregate.add_aggregate("player4", 10, false);

        auto& rigidbody = cubeObj.add_component<components::RigidBody>();

        rigidbody.create_actor(true, 4.0f);

        auto& rigidController = cubeObj.add_component<components::RigidController>();
        rigidController.lockRotations(true, false, true);
        rigidController.set_linear_damping(1.0f);
        rigidController.set_angular_damping(1.0f);

        auto material = components::Material();
        material.set_texture_slot_path(TextureType::Albedo, "oldiron/OldIron01_1K_BaseColor.png");
        material.set_texture_slot_path(TextureType::Normal, "oldiron/OldIron01_1K_Normal.png");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
        cubeObj.add_component<components::Material>(material);

        auto& client = cubeObj.add_component<components::ClientPlayer>();
        client.m_clientNum = (3);
    }
    // PLAYER 5
    {
        auto cubeObj = scene.create_game_object("stanford_dragon_5");
        cubeObj.get_component<components::Transform>().set_position(glm::vec3(0, 5.0f, 20));
        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(3, 3, 3));
        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 10, 0));
        cubeObj.add_component<components::InstanceMesh>("dragon.obj");

        auto& physics_material = cubeObj.add_component<components::PhysicsMaterial>();

        auto& shape = cubeObj.add_component<components::Shape>();
        vec3 halfsize = vec3(1.5f);
        shape.set_geometry(shape.create_cube_geometry(halfsize));

        auto& aggregate = cubeObj.add_component<components::Aggregate>();
        aggregate.add_aggregate("player5", 10, false);

        auto& rigidbody = cubeObj.add_component<components::RigidBody>();

        rigidbody.create_actor(true, 4.0f);

        auto& rigidController = cubeObj.add_component<components::RigidController>();
        rigidController.lockRotations(true, false, true);
        rigidController.set_linear_damping(1.0f);
        rigidController.set_angular_damping(1.0f);

        auto material = components::Material();
        material.set_texture_slot_path(TextureType::Albedo, "oldiron/OldIron01_1K_BaseColor.png");
        material.set_texture_slot_path(TextureType::Normal, "oldiron/OldIron01_1K_Normal.png");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
        cubeObj.add_component<components::Material>(material);

        auto& client = cubeObj.add_component<components::ClientPlayer>();
        client.m_clientNum = (4);
    }
    // PLAYER 6
    {
        auto cubeObj = scene.create_game_object("stanford_dragon_6");
        cubeObj.get_component<components::Transform>().set_position(glm::vec3(0, 5.0f, -20));
        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(3, 3, 3));
        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 10, 0));
        cubeObj.add_component<components::InstanceMesh>("dragon.obj");

        auto& physics_material = cubeObj.add_component<components::PhysicsMaterial>();

        auto& shape = cubeObj.add_component<components::Shape>();
        vec3 halfsize = vec3(1.5f);
        shape.set_geometry(shape.create_cube_geometry(halfsize));

        auto& aggregate = cubeObj.add_component<components::Aggregate>();
        aggregate.add_aggregate("player6", 10, false);

        auto& rigidbody = cubeObj.add_component<components::RigidBody>();

        rigidbody.create_actor(true, 4.0f);

        auto& rigidController = cubeObj.add_component<components::RigidController>();
        rigidController.lockRotations(true, false, true);
        rigidController.set_linear_damping(1.0f);
        rigidController.set_angular_damping(1.0f);

        auto material = components::Material();
        material.set_texture_slot_path(TextureType::Albedo, "oldiron/OldIron01_1K_BaseColor.png");
        material.set_texture_slot_path(TextureType::Normal, "oldiron/OldIron01_1K_Normal.png");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
        cubeObj.add_component<components::Material>(material);

        auto& client = cubeObj.add_component<components::ClientPlayer>();
        client.m_clientNum = (5);
    }
    {
        auto cubeObj = scene.create_game_object("post processing");
        cubeObj.get_component<components::Transform>().set_position(vec3(0));
        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(7, 7, 7));
        cubeObj.add_component<components::InstanceMesh>("postProcessPlane");
        cubeObj.add_component<components::PostProcessing>();
    }

    //    std::string filename("post_process.json");
    //    std::filesystem::path path = AssetManager::get_resources_path().append(filename);
    //    std::fstream serializedSceneStream(path);
    //
    //    serializedSceneStream.open(path, std::ios_base::out);
    //    scene.save_scene_to_stream(serializedSceneStream);
    //    serializedSceneStream.close();
    //    Scene::set_active_scene(loadedScene);
    //    serializedSceneStream.open(path, std::ios_base::in);
    //    if (serializedSceneStream) {
    //        loadedScene.load_scene_from_stream(serializedSceneStream);
    //    } else {
    //        log::debug("file not found");
    //    }
    //    serializedSceneStream.close();
    // auto demoWidget = std::make_shared<DemoWidget>("demo");
    // m_engine->get_Widget().lock()->add_widget(demoWidget);
      m_menu = std::make_shared<Menu>("menu");
      m_menu->setWinow(m_engine->get_window_module().lock()->get_window_width(),m_engine->get_window_module().lock()->get_window_height());
     m_engine->get_Widget().lock()->add_widget(m_menu);
    m_debug = std::make_shared<Debug_gui>("Debug");
    m_engine->get_Widget().lock()->add_widget(m_debug);

    m_debugPhysics = std::make_shared<DebugPhysics>("debugPhys");
    m_engine->get_Widget().lock()->add_widget(m_debugPhysics);
}

void Untie::run() {
    auto cliMod = m_engine->get_client_module().lock();
    cliMod->attempt_connection();
    while (m_engine->is_open()) {
        m_debug->set_bgfx_Time(m_engine->get_bgfx_Time_cost());
        m_debug->set_Phy_Time(m_engine->get_Phy_Time_cost());
        m_debug->set_Gui_Time(m_engine->get_Gui_Time_cost());
        m_engine->update_modules();
        auto im = m_engine->get_window_module().lock()->get_input_manager();

        if (im->key_on_trigger(KeyCode::P)) {
            if (cubeOne) {
                components::AudioSource& source = cubeOne->get_component<components::AudioSource>();
                auto weakAudioModule = m_engine->get_audio_module();
                if (!weakAudioModule.expired()) {
                    auto audioModule = weakAudioModule.lock();
                    audioModule->toggle(source);
                }
            }
        }

        if (im->key_on_trigger(KeyCode::Y)) {
            if (cubeOne) {
                components::AudioSource& source = cubeTwo->get_component<components::AudioSource>();
                auto weakAudioModule = m_engine->get_audio_module();
                if (!weakAudioModule.expired()) {
                    auto audioModule = weakAudioModule.lock();
                    audioModule->play(source);
                }
            }
        }

        if (im->key_pressed(KeyCode::Escape)) {
            m_engine->get_window_module().lock()->close();
        }
        if(m_menu->get_quit_click()){
            m_engine->get_window_module().lock()->close();
        }
        if(m_menu->get_Pause_click()){
            m_engine->switch_paused();
        }
        if (im->key_on_trigger(KeyCode::GraveAccent)) {
            if (open) {
                m_debug->setOpen(open);
            }
            if (!open) {
                m_debug->setOpen(open);
            }
            open = !open;
        }
        int64_t now = bx::getHPCounter();
        static int64_t last = now;
        const int64_t frameTime = now - last;
        last = now;
        const double freq = double(bx::getHPFrequency());
        const double toMs = 1000.0 / freq;
        m_debug->setFrame(1000 / (double(frameTime) * toMs));
    }
}

}  // namespace knot
