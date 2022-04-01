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
		m_program = myloadProgram("../shaders/", "vs", "fs_pbr");
		m_timeOffset = bx::getHPCounter();
		bx::eye4(model);
		bx::eye4(preModel);


		s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
		s_texNormal = bgfx::createUniform("s_texNormal", bgfx::UniformType::Sampler);
		s_texAORM = bgfx::createUniform("s_texAORM", bgfx::UniformType::Sampler);
		u_lightPos = bgfx::createUniform("u_lightPos", bgfx::UniformType::Vec4);
		u_lightRGB = bgfx::createUniform("u_lightRGB", bgfx::UniformType::Vec4);
		u_eyePos = bgfx::createUniform("u_eyePos", bgfx::UniformType::Vec4);
		m_mesh = meshLoad("../resource/pbr_stone/pbr_stone_mes.bin");
		m_textureColor = loadTexture("../resource/pbr_stone/pbr_stone_base_color.dds");
		m_textureNormal = loadTexture("../resource/pbr_stone/pbr_stone_normal.dds");
		m_textureAORM = loadTexture("../resource/pbr_stone/pbr_stone_aorm.tga");

		imguiCreate();
	}

	virtual int shutdown() override
	{
		imguiDestroy();

		bgfx::destroy(m_program);
		bgfx::destroy(m_textureColor);
		bgfx::destroy(m_textureNormal);
		bgfx::destroy(m_textureAORM);
		bgfx::destroy(s_texColor);
		bgfx::destroy(s_texNormal);
		bgfx::destroy(u_lightPos);
		bgfx::destroy(u_eyePos);

		meshUnload(m_mesh);

		// Shutdown bgfx.
		bgfx::shutdown();

		return 0;
	}

	bool update() override
	{
		if (!entry::processEvents(m_width, m_height, m_debug, m_reset, &m_mouseState) )
		{
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

				s = '0' + m_mesh->m_layout.has(bgfx::Attrib::Enum::TexCoord0);
				ImGui::Text(s.c_str());
				s = std::to_string(fov);
				ImGui::Text(s.c_str());
				ImGui::End();

				imguiEndFrame();
			}
			float time = (float)((bx::getHPCounter() - m_timeOffset) / double(bx::getHPFrequency()));
			// orbit camera
			{
				if (m_mouseState.m_buttons[entry::MouseButton::Left] && !down) {
					down = true;
					oldMouseX = m_mouseState.m_mx;
					oldMouseY = m_mouseState.m_my;
					rotateX = 0;
					rotateY = 0;
				}
				if (m_mouseState.m_buttons[entry::MouseButton::Left]) {
					rotateX += (m_mouseState.m_mx - oldMouseX) / 150.0;
					rotateY += (m_mouseState.m_my - oldMouseY) / 150.0;
					oldMouseX = m_mouseState.m_mx;
					oldMouseY = m_mouseState.m_my;
					bx::mtxRotateXY(tmpModel, rotateY, rotateX);
					bx::mtxMul(model, preModel, tmpModel);
					//bx::mtxRotateXY(model, 0, 0);
				}
				if (down && !m_mouseState.m_buttons[entry::MouseButton::Left]) {
					down = false;
					bx::mtxCpy(preModel, model);
				}
				fov *= pow(1.1, oldMouseZ - m_mouseState.m_mz);
				fov = bx::clamp(fov, 10.0f, 160.0f);
				oldMouseZ = m_mouseState.m_mz;
				float sensitivity = 0.3;
				if (inputGetKeyState(entry::Key::Enum::KeyW))
				{
					eye.z += sensitivity;
					at.z += sensitivity;
				}
				if (inputGetKeyState(entry::Key::Enum::KeyA))
				{
					eye.x -= sensitivity;
					at.x -= sensitivity;
				}
				if (inputGetKeyState(entry::Key::Enum::KeyS))
				{
					eye.z -= sensitivity;
					at.z -= sensitivity;
				}
				if (inputGetKeyState(entry::Key::Enum::KeyD))
				{
					eye.x += sensitivity;
					at.x += sensitivity;
				}
			}
			
			// Set view and projection matrix for view 0.
			{
				bx::mtxLookAt(view, eye, at);

				bx::mtxProj(proj, fov, float(m_width) / float(m_height), 0.1f, 10000.0f, bgfx::getCaps()->homogeneousDepth);

				bgfx::setViewTransform(0, view, proj);

				// Set view 0 default viewport.
				bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));
			}

			// This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0.
			bgfx::touch(0);			 

			bgfx::setTexture(0, s_texColor, m_textureColor);
			bgfx::setTexture(1, s_texNormal, m_textureNormal);
			bgfx::setTexture(2, s_texAORM, m_textureAORM);
			float lightPos[4] = { -10.0, -10.0, -10.0, 0.0 };
			bgfx::setUniform(u_lightPos, lightPos);
			float lightIntensity = 5000;
			float lightRGB[4] = { lightIntensity * 1, lightIntensity * 1, lightIntensity * 1, 0.0 };;
			bgfx::setUniform(u_lightRGB, lightRGB);
			float eyePos[4] = { at.x, at.y, at.z, 0.0 };
			bgfx::setUniform(u_eyePos, eyePos);
			meshSubmit(m_mesh, 0, m_program, model);

			// Submit primitive for rendering to view 0.
			bgfx::submit(0, m_program);

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
	bgfx::UniformHandle u_lightPos;
	bgfx::UniformHandle u_lightRGB;
	bgfx::UniformHandle u_eyePos;
	bgfx::ProgramHandle m_program;
	bgfx::TextureHandle m_textureColor;
	bgfx::TextureHandle m_textureNormal;
	bgfx::TextureHandle m_textureAORM;

	int32_t oldMouseX = 0;
	int32_t oldMouseY = 0;
	int32_t oldMouseZ = 0;
	float rotateX = 0; // ¶ÔÓÚÆÁÄ»µÄxy
	float rotateY = 0;
	float fov = 60;
	bool down = false;
	float model[16] = { 0 };
	float tmpModel[16] = { 0 };
	float preModel[16] = { 0 };
	float view[16] = { 0 };
	float proj[16] = { 0 };
	bx::Vec3 at = { 0.0f, 0.0f, 0.0f };
	bx::Vec3 eye = { 0.0f, 0.0f, -10.0f };
	Mesh* m_mesh;
};

} // namespace

int _main_(int _argc, char** _argv)
{
	EStarHomework app("e-star-homework", "", "");
	return entry::runApp(&app, _argc, _argv);
}

