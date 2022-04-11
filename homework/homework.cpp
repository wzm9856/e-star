/*
 * Copyright 2011-2022 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#include <bx/uint32_t.h>
#include "common.h"
#include "bgfx_utils.h"
#include "bgfx_logo.h"
#include "imgui/imgui.h"
#include <string>
#include "../bgfx/examples/common/entry/input.h"

namespace
{

struct PosColorVertex
{
	float m_x;
	float m_y;
	float m_z;

	static void init()
	{
		ms_layout.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float).end();
	};

	static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout PosColorVertex::ms_layout;

static PosColorVertex s_cubeVertices[] =
{
	{-1.0f,  1.0f,  1.0f},
	{ 1.0f,  1.0f,  1.0f},
	{-1.0f, -1.0f,  1.0f},
	{ 1.0f, -1.0f,  1.0f},
	{-1.0f,  1.0f, -1.0f},
	{ 1.0f,  1.0f, -1.0f},
	{-1.0f, -1.0f, -1.0f},
	{ 1.0f, -1.0f, -1.0f},
};

static const uint16_t s_cubeTriList[] =
{
	0, 1, 2, // 0
	1, 3, 2,
	4, 6, 5, // 2
	5, 6, 7,
	0, 2, 4, // 4
	4, 2, 6,
	1, 5, 3, // 6
	5, 7, 3,
	0, 4, 1, // 8
	4, 5, 1,
	2, 3, 6, // 10
	6, 3, 7,
};
class EStarHomework : public entry::AppI
{
public:
	EStarHomework(const char* _name, const char* _description, const char* _url)
		: entry::AppI(_name, _description, _url)
	{
		m_width = 0;
		m_height = 0;
		m_debug = BGFX_DEBUG_NONE;
		m_reset = BGFX_RESET_NONE;
	}

	void init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override
	{
		Args args(_argc, _argv);

		m_width  = _width;
		m_height = _height;
		m_debug  = BGFX_DEBUG_TEXT;
		m_reset  = BGFX_RESET_VSYNC;

		bgfx::Init init;
		//init.type     = args.m_type;
		init.type = bgfx::RendererType::OpenGL;
		init.vendorId = args.m_pciId;
		init.resolution.width  = m_width;
		init.resolution.height = m_height;
		init.resolution.reset  = m_reset;
		bgfx::init(init);

		// Enable debug text.
		bgfx::setDebug(m_debug);

		// Set view 0 clear state.
		bgfx::setViewClear(0
			, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
			, 0x303030ff
			, 1.0f
			, 0
			);

		// Create program from shaders.
		m_program = myloadProgram("../shaders/ibl/", "vs", "fs_pbr");
		m_skyboxProgram = myloadProgram("../shaders/ibl/", "vs_skybox", "fs_skybox");
		m_timeOffset = bx::getHPCounter();
		PosColorVertex::init();
		bx::mtxIdentity(m_model);
		bx::mtxIdentity(m_preModel);
		bx::mtxIdentity(m_identity);

		s_texColor =	bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
		s_texNormal =	bgfx::createUniform("s_texNormal", bgfx::UniformType::Sampler);
		s_texAORM =		bgfx::createUniform("s_texAORM", bgfx::UniformType::Sampler);
		s_texSkyLod =	bgfx::createUniform("s_texSkyLod", bgfx::UniformType::Sampler);
		s_texSkyIrr =	bgfx::createUniform("s_texSkyIrr", bgfx::UniformType::Sampler);
		s_texBrdfLut =	bgfx::createUniform("s_texBrdfLut", bgfx::UniformType::Sampler);
		m_textureColor =	loadTexture("../resource/pbr_stone/pbr_stone_base_color.dds");
		m_textureNormal =	loadTexture("../resource/pbr_stone/pbr_stone_normal.dds");
		m_textureAORM =		loadTexture("../resource/pbr_stone/pbr_stone_aorm.tga");
		m_textureSkyLod =	loadTexture("../resource/env_maps/kyoto_lod.dds");
		m_textureSkyIrr =	loadTexture("../resource/env_maps/kyoto_irr.dds");
		m_textureBrdfLut =	loadTexture("../resource/pbr_stone/brdflut.dds");

		u_lightPos =	bgfx::createUniform("u_lightPos", bgfx::UniformType::Vec4);
		u_lightRGB =	bgfx::createUniform("u_lightRGB", bgfx::UniformType::Vec4);
		u_eyePos =		bgfx::createUniform("u_eyePos", bgfx::UniformType::Vec4);
		m_mesh =		meshLoad("../resource/pbr_stone/pbr_stone_mes.bin");

		m_skyboxVBO = bgfx::createVertexBuffer(bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)), PosColorVertex::ms_layout);
		m_skyboxIBO = bgfx::createIndexBuffer(bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList)));

		imguiCreate();
	}

	virtual int shutdown() override
	{
		imguiDestroy();

		bgfx::destroy(m_program);
		bgfx::destroy(m_skyboxProgram);
		bgfx::destroy(m_textureColor);
		bgfx::destroy(m_textureNormal);
		bgfx::destroy(m_textureAORM);
		bgfx::destroy(m_textureSkyLod);
		bgfx::destroy(m_textureSkyIrr);
		bgfx::destroy(m_textureBrdfLut);
		bgfx::destroy(s_texColor);
		bgfx::destroy(s_texNormal);
		bgfx::destroy(s_texAORM);
		bgfx::destroy(s_texSkyLod);
		bgfx::destroy(s_texSkyIrr);
		bgfx::destroy(s_texBrdfLut);
		bgfx::destroy(u_lightPos);
		bgfx::destroy(u_lightRGB);
		bgfx::destroy(u_eyePos);

		meshUnload(m_mesh);

		// Shutm_buttonDown bgfx.
		bgfx::shutdown();

		return 0;
	}

	bool update() override
	{
		if (!entry::processEvents(m_width, m_height, m_debug, m_reset, &m_mouseState) )
		{
			float time = (float)((bx::getHPCounter() - m_timeOffset) / double(bx::getHPFrequency()));

			// Set GUI
			{
				imguiBeginFrame(m_mouseState.m_mx
					, m_mouseState.m_my
					, (m_mouseState.m_buttons[entry::MouseButton::Left] ? IMGUI_MBUT_LEFT : 0)
					| (m_mouseState.m_buttons[entry::MouseButton::Right] ? IMGUI_MBUT_RIGHT : 0)
					| (m_mouseState.m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0)
					, m_mouseState.m_mz
					, uint16_t(m_width)
					, uint16_t(m_height)
				);

				showExampleDialog(this);

				ImGui::SetNextWindowPos(
					ImVec2(m_width - m_width / 5.0f - 10.0f, 10.0f)
					, ImGuiCond_FirstUseEver
				);
				ImGui::SetNextWindowSize(
					ImVec2(m_width / 5.0f, m_height / 3.5f)
					, ImGuiCond_FirstUseEver
				);
				ImGui::Begin("Info", NULL, 0);

				std::string s = std::to_string(m_mouseState.m_mx) + " " + std::to_string(m_mouseState.m_my) + " " +
					std::to_string(m_mouseState.m_mz) + " " + std::to_string(m_mouseState.m_buttons[0]) + " " +
					std::to_string(m_mouseState.m_buttons[1]) + " " + std::to_string(m_mouseState.m_buttons[2]) + " " +
					std::to_string(m_mouseState.m_buttons[3]);
				ImGui::Text(s.c_str());

				s = std::to_string(m_at.x) + " " + std::to_string(m_at.y) + " " + std::to_string(m_at.z);
				ImGui::Text(s.c_str());
				s = std::to_string(m_fov);
				ImGui::Text(s.c_str());
				ImGui::End();

				imguiEndFrame();
			}

			// orbit camera+生成M矩阵
			{
				if (m_mouseState.m_buttons[entry::MouseButton::Left] && !m_buttonDown) {
					m_buttonDown = true;
					m_oldMouseX = m_mouseState.m_mx;
					m_oldMouseY = m_mouseState.m_my;
					m_rotateX = 0;
					m_rotateY = 0;
				}
				if (m_mouseState.m_buttons[entry::MouseButton::Left]) {
					m_rotateX += (m_mouseState.m_mx - m_oldMouseX) / 150.0;
					m_rotateY += (m_mouseState.m_my - m_oldMouseY) / 150.0;
					m_oldMouseX = m_mouseState.m_mx;
					m_oldMouseY = m_mouseState.m_my;
					bx::mtxRotateXY(m_tmpModel, m_rotateY, m_rotateX);
					bx::mtxMul(m_model, m_preModel, m_tmpModel);
				}
				if (m_buttonDown && !m_mouseState.m_buttons[entry::MouseButton::Left]) {
					m_buttonDown = false;
					bx::mtxCpy(m_preModel, m_model);
				}
				m_fov *= pow(1.1, m_oldMouseZ - m_mouseState.m_mz);
				m_fov = bx::clamp(m_fov, 10.0f, 160.0f);
				m_oldMouseZ = m_mouseState.m_mz;
				float sensitivity = 0.3;
				if (inputGetKeyState(entry::Key::Enum::KeyW))
				{
					m_eye.z += sensitivity;
					m_at.z += sensitivity;
				}
				if (inputGetKeyState(entry::Key::Enum::KeyA))
				{
					m_eye.x -= sensitivity;
					m_at.x -= sensitivity;
				}
				if (inputGetKeyState(entry::Key::Enum::KeyS))
				{
					m_eye.z -= sensitivity;
					m_at.z -= sensitivity;
				}
				if (inputGetKeyState(entry::Key::Enum::KeyD))
				{
					m_eye.x += sensitivity;
					m_at.x += sensitivity;
				}
			}

			bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height)); // view0给天空盒用
			bgfx::setViewRect(1, 0, 0, uint16_t(m_width), uint16_t(m_height)); // view1给模型用

			bgfx::setTexture(0, s_texColor, m_textureColor);
			bgfx::setTexture(1, s_texNormal, m_textureNormal);
			bgfx::setTexture(2, s_texAORM, m_textureAORM);
			bgfx::setTexture(3, s_texBrdfLut, m_textureBrdfLut);
			bgfx::setTexture(4, s_texSkyLod, m_textureSkyLod);
			bgfx::setTexture(5, s_texSkyIrr, m_textureSkyIrr);

			// 画模型+生成VP矩阵
			{
				bx::mtxLookAt(m_view, m_eye, m_at);
				bx::mtxProj(m_proj, m_fov, float(m_width) / float(m_height), 0.1f, 2000.0f, bgfx::getCaps()->homogeneousDepth);
				bgfx::setViewTransform(1, m_view, m_proj);
				float lightPos[4] = { -10.0, -10.0, -10.0, 0.0 };
				bgfx::setUniform(u_lightPos, lightPos);
				float lightIntensity = 0;
				float lightRGB[4] = { lightIntensity * 1, lightIntensity * 1, lightIntensity * 1, 0.0 };;
				bgfx::setUniform(u_lightRGB, lightRGB);
				float eyePos[4] = { m_eye.x, m_eye.y, m_eye.z, 0.0 };
				bgfx::setUniform(u_eyePos, eyePos);
				meshSubmit(m_mesh, 1, m_program, m_model);
			}
			
			// 画天空盒
			{
				bgfx::setVertexBuffer(0, m_skyboxVBO);
				bgfx::setIndexBuffer(m_skyboxIBO);
				bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
				bgfx::setTransform(m_model); // 理论上此处是需要去掉平移量，但是本来model也没平移，只是纯旋转
				bgfx::setViewTransform(0, m_identity, m_proj);
				bgfx::submit(0, m_skyboxProgram);
			}

			// Advance to next frame. Rendering thread will be kicked to process submitted rendering primitives.
			bgfx::frame();
			
			return true;
		}

		return false;
	}

	entry::MouseState m_mouseState;

	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_debug;
	uint32_t m_reset;
	int64_t m_timeOffset;
	int32_t m_pt = 0;

	bgfx::UniformHandle s_texColor;
	bgfx::UniformHandle s_texNormal;
	bgfx::UniformHandle s_texAORM;
	bgfx::UniformHandle s_texSkyLod;
	bgfx::UniformHandle s_texSkyIrr;
	bgfx::UniformHandle s_texBrdfLut;
	bgfx::UniformHandle u_lightPos;
	bgfx::UniformHandle u_lightRGB;
	bgfx::UniformHandle u_eyePos;
	bgfx::ProgramHandle m_program;
	bgfx::ProgramHandle m_skyboxProgram;
	bgfx::TextureHandle m_textureColor;
	bgfx::TextureHandle m_textureNormal;
	bgfx::TextureHandle m_textureAORM;
	bgfx::TextureHandle m_textureSkyLod;
	bgfx::TextureHandle m_textureSkyIrr;
	bgfx::TextureHandle m_textureBrdfLut;

	bgfx::VertexBufferHandle m_skyboxVBO;
	bgfx::IndexBufferHandle  m_skyboxIBO;

	int32_t m_oldMouseX = 0;
	int32_t m_oldMouseY = 0;
	int32_t m_oldMouseZ = 0;
	float	m_rotateX = 0; // 是对于屏幕的xy
	float	m_rotateY = 0;
	float	m_fov = 60;
	bool	m_buttonDown = false;
	float	m_model[16] = { 0 }; // 无论什么情况，最终设置给shader的M矩阵都是m_model
	float	m_tmpModel[16] = { 0 }; //当鼠标按下没松开时，把m_tmpModel设置成当前拖动旋转的矩阵，乘上按下鼠标之前的m_model（被复制到了m_preModel），赋给了m_model
	float	m_preModel[16] = { 0 }; //当鼠标松开时，把m_model复制给m_premodel
	float	m_view[16] = { 0 };
	float	m_proj[16] = { 0 };
	float	m_identity[16] = { 0 };
	bx::Vec3 m_at = { 0.0f, 0.0f, 0.0f }; //at是视点，eye是相机坐标
	bx::Vec3 m_eye = { 0.0f, 0.0f, -10.0f };
	Mesh*	m_mesh;
};

} // namespace

int _main_(int _argc, char** _argv)
{
	system("..\\shaders\\compileShader.bat");
	EStarHomework app("e-star-homework", "", "");
	return entry::runApp(&app, _argc, _argv);
	
}

