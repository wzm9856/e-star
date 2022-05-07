/*
 * Copyright 2011-2022 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#include <bx/uint32_t.h>
#include "common.h"
#include "bgfx_utils.h"
#include "imgui/imgui.h"
#include <string>
#include "../bgfx/examples/common/entry/input.h"
#include <iostream>
typedef bx::Quaternion Vec4;
namespace
{

struct SkyboxVertex
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

bgfx::VertexLayout SkyboxVertex::ms_layout;

static SkyboxVertex s_skyboxVertices[] =
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

static const uint16_t s_skyboxTriList[] =
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

struct GroundVertex
{
	float m_x;
	float m_y;
	float m_z;
	float m_nx;
	float m_ny;
	float m_nz;
	uint32_t m_abgr;

	static void init()
	{
		ms_layout.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
			.end();
	};

	static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout GroundVertex::ms_layout;

// 因为想要各面上的法线保持正常，所以不能复用天空盒的顶点坐标
static GroundVertex s_groundVertices[] =
{
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0xff33333333,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0xff33333333,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0xff33333333,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0xff33333333,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0xff33333333,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0xff33333333,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0xff33333333,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0xff33333333,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0xff33333333,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0xff33333333,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0xff33333333,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0xff33333333,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0xff33333333,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0xff33333333,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0xff33333333,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0xff33333333,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0xff33333333,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0xff33333333,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0xff33333333,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0xff33333333,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0xff33333333,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0xff33333333,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0xff33333333,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0xff33333333,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0xff33333333,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0xff33333333,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0xff33333333,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0xff33333333,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0xff33333333,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0xff33333333,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0xff33333333,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0xff33333333,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0xff33333333,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0xff33333333,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0xff33333333,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0xff33333333
};

// 用于画一个深度图像出来的两个三角形
struct TestVertex
{
	float m_x;
	float m_y;
	float m_z;
	float m_u;
	float m_v;

	static void init()
	{
		ms_layout.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();
	};

	static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout TestVertex::ms_layout;

// 因为想要各面上的法线保持正常，所以不能复用天空盒的顶点坐标
static TestVertex s_testVertices[] =
{
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,
		 1.0f,  1.0f, -1.0f,  1.0f,  1.0f,
};

void VBOSubmit(const MeshState* _state, const float* _transform, bgfx::VertexBufferHandle VBO, bgfx::IndexBufferHandle IBO = BGFX_INVALID_HANDLE) {
	BX_ASSERT(bgfx::isValid(VBO), "VBO is not valid");
	BX_ASSERT(IBO.idx == UINT16_MAX || bgfx::isValid(IBO), "IBO is not valid"); //uint16max是invalid handle的值
	BX_ASSERT(_transform != 0, "transform is not valid");
	for (uint8_t tex = 0; tex < _state->m_numTextures; ++tex)
	{
		const MeshState::Texture& texture = _state->m_textures[tex];
		bgfx::setTexture(
			texture.m_stage
			, texture.m_sampler
			, texture.m_texture
			, texture.m_flags
		);
	}
	bgfx::setVertexBuffer(0, VBO);
	if (IBO.idx != UINT16_MAX) bgfx::setIndexBuffer(IBO);
	bgfx::setTransform(_transform); // 理论上此处是需要去掉平移量，但是本来model也没平移，只是纯旋转
	bgfx::setState(_state->m_state);
	bgfx::submit(_state->m_viewId, _state->m_program, 0, BGFX_DISCARD_INDEX_BUFFER | BGFX_DISCARD_VERTEX_STREAMS);
	bgfx::discard(BGFX_DISCARD_BINDINGS | BGFX_DISCARD_STATE | BGFX_DISCARD_TRANSFORM);
}

static const char* s_levelName[]{
	"pbr",
	"bling-phong",
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
		init.type = bgfx::RendererType::Direct3D11;
		//init.type = bgfx::RendererType::OpenGL;
		//init.vendorId = args.m_pciId;
		init.resolution.width  = m_width;
		init.resolution.height = m_height;
		init.resolution.reset  = m_reset;
		bgfx::init(init);

		if(init.type == bgfx::RendererType::OpenGL)
			system("..\\shaders\\compileShaderGLSL.bat");
		else if(init.type == bgfx::RendererType::Direct3D11)
			system("..\\shaders\\compileShaderHLSL.bat");

		// Enable debug text.
		bgfx::setDebug(m_debug);

		bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH);
		bgfx::setViewClear(1
			, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
			, 0x303030ff
			, 1.0f
			, 0
			);
		bgfx::setViewClear(3, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH);

		// Create program from shaders.
		m_pbrProgram = myloadProgram("../shaders/ibl/", "vs_pbr", "fs_pbr", init.type);
		m_pbrShadowProgram = myloadProgram("../shaders/ibl/", "vs_pbr", "fs_pbr_shadowMap", init.type);
		m_blpProgram = myloadProgram("../shaders/bling-phong/", "vs", "fs_bling-phong", init.type);
		m_blpShadowProgram = myloadProgram("../shaders/bling-phong/", "vs", "fs_bling-phong_shadowMap", init.type);
		m_skyboxProgram = myloadProgram("../shaders/ibl/", "vs_skybox", "fs_skybox", init.type);
		m_groundProgram = myloadProgram("../shaders/shadowMap/ground/", "vs_ground", "fs_ground", init.type);
		m_shadowProgram = myloadProgram("../shaders/shadowMap/shadow/", "vs_shadow", "fs_shadow", init.type);
		m_testProgram	= myloadProgram("../shaders/shadowMap/test/", "vs_test", "fs_test", init.type);
		BX_ASSERT(bgfx::isValid(m_pbrProgram), "m_pbrProgram is not valid");
		BX_ASSERT(bgfx::isValid(m_blpProgram), "m_blpProgram is not valid");
		BX_ASSERT(bgfx::isValid(m_pbrShadowProgram), "m_pbrShadowProgram is not valid");
		BX_ASSERT(bgfx::isValid(m_blpShadowProgram), "m_blpShadowProgram is not valid");
		BX_ASSERT(bgfx::isValid(m_skyboxProgram), "m_skyboxProgram is not valid");
		BX_ASSERT(bgfx::isValid(m_groundProgram), "m_groundProgram is not valid");
		BX_ASSERT(bgfx::isValid(m_shadowProgram), "m_shadowProgram is not valid");
		BX_ASSERT(bgfx::isValid(m_testProgram), "m_testProgram is not valid");

		m_timeOffset = bx::getHPCounter();
		SkyboxVertex::init();
		GroundVertex::init();
		TestVertex::init();
		bx::mtxIdentity(m_model);
		bx::mtxIdentity(m_preModel);
		bx::mtxIdentity(m_identity);
		bx::mtx4toMtx3(m_normalMtx, m_identity);
		m_groundScale[0] = 30;
		m_groundScale[5] = 1;
		m_groundScale[13] = 0;//平面和石头的距离
		m_groundScale[10] = 30;
		m_groundScale[15] = 1;

		s_texColor	= bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
		s_texNormal = bgfx::createUniform("s_texNormal", bgfx::UniformType::Sampler);
		s_texAORM	= bgfx::createUniform("s_texAORM", bgfx::UniformType::Sampler);
		s_texSkyLod = bgfx::createUniform("s_texSkyLod", bgfx::UniformType::Sampler);
		s_texSkyIrr = bgfx::createUniform("s_texSkyIrr", bgfx::UniformType::Sampler);
		s_texBrdfLut= bgfx::createUniform("s_texBrdfLut", bgfx::UniformType::Sampler);
		s_shadowMap = bgfx::createUniform("s_shadowMap", bgfx::UniformType::Sampler);
		m_textureColor	= loadTexture("../resource/pbr_stone/pbr_stone_base_color.dds");
		m_textureNormal	= loadTexture("../resource/pbr_stone/pbr_stone_normal.dds");
		m_textureAORM	= loadTexture("../resource/pbr_stone/pbr_stone_aorm.tga");
		m_textureSkyLod	= loadTexture("../resource/env_maps/kyoto_lod.dds");
		m_textureSkyIrr	= loadTexture("../resource/env_maps/kyoto_irr.dds");
		m_textureBlackLod=loadTexture("../resource/env_maps/black_lod.dds");
		m_textureBlackIrr=loadTexture("../resource/env_maps/black_irr.dds");
		m_textureBrdfLut= loadTexture("../resource/pbr_stone/brdflut.dds");
		BX_ASSERT(bgfx::isValid(m_textureColor), "m_textureColor is not valid");
		BX_ASSERT(bgfx::isValid(m_textureNormal), "m_textureNormal is not valid");
		BX_ASSERT(bgfx::isValid(m_textureAORM), "m_textureAORM is not valid");
		BX_ASSERT(bgfx::isValid(m_textureSkyLod), "m_textureSkyLod is not valid");
		BX_ASSERT(bgfx::isValid(m_textureSkyIrr), "m_textureSkyIrr is not valid");
		BX_ASSERT(bgfx::isValid(m_textureBlackLod), "m_textureBlackLod is not valid");
		BX_ASSERT(bgfx::isValid(m_textureBlackIrr), "m_textureBlackIrr is not valid");
		BX_ASSERT(bgfx::isValid(m_textureBrdfLut), "m_textureBrdfLut is not valid");
		m_shadowMap	= bgfx::createTexture2D(512, 512, false, 1, bgfx::TextureFormat::D16, BGFX_TEXTURE_RT | BGFX_SAMPLER_COMPARE_LEQUAL);
		m_shadowMapInRGB = bgfx::createTexture2D(512, 512, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT);
		bgfx::TextureHandle texturesForShadowFBO[] = { m_shadowMap, m_shadowMapInRGB };
		m_shadowMapFBO = bgfx::createFrameBuffer(BX_COUNTOF(texturesForShadowFBO), texturesForShadowFBO, true);
		BX_ASSERT(bgfx::isValid(m_shadowMapFBO), "m_shadowMapFBO is not valid");

		u_lightPos =	bgfx::createUniform("u_lightPos", bgfx::UniformType::Vec4);
		u_lightRGB =	bgfx::createUniform("u_lightRGB", bgfx::UniformType::Vec4);
		u_lightMtx =	bgfx::createUniform("u_lightMtx", bgfx::UniformType::Mat4);
		u_eyePos =		bgfx::createUniform("u_eyePos", bgfx::UniformType::Vec4);
		u_normalMtx =	bgfx::createUniform("u_normalMtx", bgfx::UniformType::Mat3);
		m_mesh =		meshLoad("../resource/pbr_stone/pbr_stone.bin");
		//m_mesh =		meshLoad("../resource/pbr_stone/pbr_stone_mes.bin");

		m_skyboxVBO = bgfx::createVertexBuffer(bgfx::makeRef(s_skyboxVertices, sizeof(s_skyboxVertices)), SkyboxVertex::ms_layout);
		m_skyboxIBO = bgfx::createIndexBuffer(bgfx::makeRef(s_skyboxTriList, sizeof(s_skyboxTriList)));
		m_groundVBO = bgfx::createVertexBuffer(bgfx::makeRef(s_groundVertices, sizeof(s_groundVertices)), GroundVertex::ms_layout);
		m_testVBO = bgfx::createVertexBuffer(bgfx::makeRef(s_testVertices, sizeof(s_testVertices)), TestVertex::ms_layout);

		imguiCreate();

		// state0->view0 渲染shadowmap
		m_state[0] = meshStateCreate();
		m_state[0]->m_state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS;
		m_state[0]->m_program = m_shadowProgram;
		m_state[0]->m_viewId = 0;
		m_state[0]->m_numTextures = 0;

		// state1->view1 渲染天空盒
		m_state[1] = meshStateCreate();
		m_state[1]->m_state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z ;
		m_state[1]->m_program = m_skyboxProgram;
		m_state[1]->m_viewId = 1;
		m_state[1]->m_numTextures = 1;
		m_state[1]->m_textures[0].m_flags = UINT32_MAX;
		m_state[1]->m_textures[0].m_stage = 0;
		m_state[1]->m_textures[0].m_sampler = s_texSkyLod;
		m_state[1]->m_textures[0].m_texture = m_textureSkyLod;

		// state2->view2 渲染石头
		m_state[2] = meshStateCreate();
		m_state[2]->m_state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CCW;
		m_state[2]->m_program = m_pbrProgram;
		m_state[2]->m_viewId = 2;
		m_state[2]->m_numTextures = 7;
		for (int i = 0; i < m_state[2]->m_numTextures; i++) {
			m_state[2]->m_textures[i].m_flags = UINT32_MAX;
			m_state[2]->m_textures[i].m_stage = i+1;
		}
		m_state[2]->m_textures[0].m_sampler = s_texColor;
		m_state[2]->m_textures[0].m_texture = m_textureColor;
		m_state[2]->m_textures[1].m_sampler = s_texNormal;
		m_state[2]->m_textures[1].m_texture = m_textureNormal;
		m_state[2]->m_textures[2].m_sampler = s_shadowMap;
		m_state[2]->m_textures[2].m_texture = m_shadowMap;
		m_state[2]->m_textures[3].m_sampler = s_texSkyLod;
		m_state[2]->m_textures[3].m_texture = m_textureSkyLod;
		m_state[2]->m_textures[4].m_sampler = s_texSkyIrr;
		m_state[2]->m_textures[4].m_texture = m_textureSkyIrr;
		m_state[2]->m_textures[5].m_sampler = s_texBrdfLut;
		m_state[2]->m_textures[5].m_texture = m_textureBrdfLut;
		m_state[2]->m_textures[6].m_sampler = s_texAORM;
		m_state[2]->m_textures[6].m_texture = m_textureAORM;

		// state3->view2 渲染地面
		m_state[3] = meshStateCreate();
		m_state[3]->m_state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS;
		m_state[3]->m_program = m_groundProgram;
		m_state[3]->m_viewId = 2;
		m_state[3]->m_numTextures = 1;
		m_state[3]->m_textures[0].m_flags = UINT32_MAX;
		m_state[3]->m_textures[0].m_stage = 0;
		m_state[3]->m_textures[0].m_sampler = s_shadowMap;
		m_state[3]->m_textures[0].m_texture = m_shadowMap;

		// state4 ->view3 渲染深度图
		m_state[4] = meshStateCreate();
		m_state[4]->m_state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_DEPTH_TEST_LESS;
		m_state[4]->m_program = m_testProgram;
		m_state[4]->m_viewId = 3;
		m_state[4]->m_numTextures = 1;
		m_state[4]->m_textures[0].m_flags = UINT32_MAX;
		m_state[4]->m_textures[0].m_stage = 0;
		m_state[4]->m_textures[0].m_sampler = s_shadowMap;
		m_state[4]->m_textures[0].m_texture = m_shadowMapInRGB;
	}

	virtual int shutdown() override
	{
		imguiDestroy();

		bgfx::destroy(m_pbrProgram);
		bgfx::destroy(m_pbrShadowProgram);
		bgfx::destroy(m_blpProgram);
		bgfx::destroy(m_blpShadowProgram);
		bgfx::destroy(m_skyboxProgram);
		bgfx::destroy(m_groundProgram);
		bgfx::destroy(m_shadowProgram);
		bgfx::destroy(m_testProgram);
		bgfx::destroy(m_shadowMapFBO);
		bgfx::destroy(m_textureColor);
		bgfx::destroy(m_textureNormal);
		bgfx::destroy(m_textureAORM);
		bgfx::destroy(m_textureSkyLod);
		bgfx::destroy(m_textureSkyIrr);
		bgfx::destroy(m_textureBlackLod);
		bgfx::destroy(m_textureBlackIrr);
		bgfx::destroy(m_textureBrdfLut);
		bgfx::destroy(m_shadowMap);
		bgfx::destroy(m_shadowMapInRGB);
		bgfx::destroy(s_texColor);
		bgfx::destroy(s_texNormal);
		bgfx::destroy(s_texAORM);
		bgfx::destroy(s_texSkyLod);
		bgfx::destroy(s_texSkyIrr);
		bgfx::destroy(s_texBrdfLut);
		bgfx::destroy(s_shadowMap);
		bgfx::destroy(u_normalMtx);
		bgfx::destroy(u_lightPos);
		bgfx::destroy(u_lightRGB);
		bgfx::destroy(u_lightMtx);
		bgfx::destroy(u_eyePos);
		for(int i = 0; i<BX_COUNTOF(m_state); i++)
			meshStateDestroy(m_state[i]);
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
				ImGui::Begin("Options", NULL, 0);
				ImGui::Combo("##着色模型", &m_pt, s_levelName, BX_COUNTOF(s_levelName));
				ImGui::Checkbox("shadowmap?", &m_shadowOn);
				ImGui::Checkbox("lights on?", &m_lightsOn);
				ImGui::Checkbox("ibl?", &m_iblOn);

				if (m_pt == 1 && m_iblOn) {
					ImGui::Text("IBL while using bling-phong is not");
					ImGui::Text("implemented, please use PBR instead.");
				}
					
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
				bx::mtxMul(m_groundModel, m_groundScale, m_model);
			}

			bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height)); // view0渲染相机视角深度
			bgfx::setViewRect(1, 0, 0, uint16_t(m_width), uint16_t(m_height)); // view1渲染天空盒
			bgfx::setViewRect(2, 0, 0, uint16_t(m_width), uint16_t(m_height)); // view2渲染模型
			bgfx::setViewRect(3, 0, uint16_t(m_height / 3*2), uint16_t(m_width/3), uint16_t(m_height/3)); // view3渲染深度图
			bgfx::setViewFrameBuffer(0, m_shadowMapFBO);

			// 生成光源VP矩阵
			{
				bx::mtxLookAt(m_lightView, m_lighteye.getVec3(), m_lightat.getVec3());
				bx::mtxProj(m_lightProj, 70.0f, 1.0f, 10.0f, 50.0f, bgfx::getCaps()->homogeneousDepth);
				bx::mtxMul(m_lightMtx, m_lightView, m_lightProj);
			}

			// 生成相机VP矩阵
			{
				bx::mtxLookAt(m_view, m_eye.getVec3(), m_at.getVec3());
				bx::mtxProj(m_proj, m_fov, float(m_width) / float(m_height), 0.1f, 2000.0f, bgfx::getCaps()->homogeneousDepth);
				bgfx::setViewTransform(m_state[2]->m_viewId, m_view, m_proj);
				bgfx::setUniform(u_lightPos, &m_lighteye);
				bgfx::setUniform(u_lightRGB, &m_lightrgb);
				bgfx::setUniform(u_eyePos, &m_eye);
			}
			
			// 绘制深度图和地面
			if (m_shadowOn) {
				bgfx::setViewTransform(m_state[0]->m_viewId, m_lightView, m_lightProj);
				meshSubmit(m_mesh, &m_state[0], 1, m_model);
				VBOSubmit(m_state[0], m_groundModel, m_groundVBO);

				// 绘制shadowmap示意图
				VBOSubmit(m_state[4], m_identity, m_testVBO);

				bgfx::setViewTransform(2, m_view, m_proj);
				bgfx::setUniform(u_lightMtx, m_lightMtx);
				VBOSubmit(m_state[3], m_groundModel, m_groundVBO);
			}


			// 生成normalMtx，对于某一帧，其各处的normalMtx都一样，一次计算多次满足
			bx::mtx4toMtx3(m_normalMtx, m_model);
			bx::mtx3Inverse(m_normalMtx, m_normalMtx);
			bx::mtx3TransposeInPlace(m_normalMtx);

			if (m_iblOn) {
				m_state[1]->m_textures[0].m_texture = m_textureSkyLod;
				m_state[2]->m_textures[3].m_texture = m_textureSkyLod;
				m_state[2]->m_textures[4].m_texture = m_textureSkyIrr;
			}
			else {
				m_state[1]->m_textures[0].m_texture = m_textureBlackLod;
				m_state[2]->m_textures[3].m_texture = m_textureBlackLod;
				m_state[2]->m_textures[4].m_texture = m_textureBlackIrr;
			}

			bgfx::setUniform(u_normalMtx, m_normalMtx);
			bgfx::setViewTransform(m_state[1]->m_viewId, m_identity, m_proj);
			VBOSubmit(m_state[1], m_model, m_skyboxVBO, m_skyboxIBO);// 理论上此处model是需要去掉平移量，但是本来model也没平移，只是纯旋转

			if (m_pt == 1) {
				m_lightrgb = { 1000 * (float)m_lightsOn,1000 * (float)m_lightsOn,1000 * (float)m_lightsOn,0};
				if (m_shadowOn) {
					bgfx::setUniform(u_lightMtx, m_lightMtx);
					m_state[2]->m_program = m_blpShadowProgram;
				}
				else
					m_state[2]->m_program = m_blpProgram;
			}
			else if (m_pt == 0) {
				m_lightrgb = { 3000 * (float)m_lightsOn,3000 * (float)m_lightsOn,3000 * (float)m_lightsOn,0 };
				if (m_shadowOn) {
					bgfx::setUniform(u_lightMtx, m_lightMtx);
					m_state[2]->m_program = m_pbrShadowProgram;
				}
				else
					m_state[2]->m_program = m_pbrProgram;
			}
			meshSubmit(m_mesh, &m_state[2], 1, m_model);

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
	int m_pt = 0;
	bool m_shadowOn = false;
	bool m_lightsOn = true;
	bool m_iblOn = true;

	bgfx::UniformHandle s_texColor;
	bgfx::UniformHandle s_texNormal;
	bgfx::UniformHandle s_texAORM;
	bgfx::UniformHandle s_texSkyLod;
	bgfx::UniformHandle s_texSkyIrr;
	bgfx::UniformHandle s_texBrdfLut;
	bgfx::UniformHandle s_shadowMap;
	bgfx::UniformHandle u_lightPos;
	bgfx::UniformHandle u_lightRGB;
	bgfx::UniformHandle u_lightMtx;
	bgfx::UniformHandle u_eyePos;
	bgfx::UniformHandle u_normalMtx;
	bgfx::ProgramHandle m_pbrProgram;
	bgfx::ProgramHandle m_pbrShadowProgram;
	bgfx::ProgramHandle m_blpProgram;
	bgfx::ProgramHandle m_blpShadowProgram;
	bgfx::ProgramHandle m_skyboxProgram;
	bgfx::ProgramHandle m_groundProgram;
	bgfx::ProgramHandle m_shadowProgram;
	bgfx::ProgramHandle m_testProgram;
	bgfx::TextureHandle m_textureColor;
	bgfx::TextureHandle m_textureNormal;
	bgfx::TextureHandle m_textureAORM;
	bgfx::TextureHandle m_textureSkyLod;
	bgfx::TextureHandle m_textureSkyIrr;
	bgfx::TextureHandle m_textureBlackLod;
	bgfx::TextureHandle m_textureBlackIrr;
	bgfx::TextureHandle m_textureBrdfLut;
	bgfx::TextureHandle m_shadowMap;
	bgfx::TextureHandle m_shadowMapInRGB;

	bgfx::FrameBufferHandle m_shadowMapFBO;

	bgfx::VertexBufferHandle m_skyboxVBO;
	bgfx::IndexBufferHandle  m_skyboxIBO;
	bgfx::VertexBufferHandle m_groundVBO;
	bgfx::VertexBufferHandle m_testVBO;

	MeshState* m_state[5];

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
	float	m_identity[16] = { 0 }; //保持他identity的身份，做一个工具人
	float	m_groundScale[16] = { 0 }; // 用于拉伸地面模型
	float	m_groundModel[16] = { 0 };
	float	m_lightView[16] = { 0 };
	float	m_lightProj[16] = { 0 };
	float	m_lightMtx[16] = { 0 };	
	float	m_normalMtx[9] = { 0 };
	Vec4 m_at = { 0.0f, 0.0f, 0.0f, 0.0f }; //at是视点，eye是相机坐标
	Vec4 m_eye = { 0.0f, 0.0f, -20.0f, 0.0f };
	Vec4 m_lightat = { 0.0f, 0.0f, 0.0f, 0.0f }; //at是视点，eye是光源坐标
	Vec4 m_lighteye = { -20.0f, -20.0f, -20.0f, 0.0f };
	Vec4 m_lightrgb = { 1000,1000,1000,0 };
	Mesh*	m_mesh;
};

} // namespace

int _main_(int _argc, char** _argv)
{
	EStarHomework app("e-star-homework", "", "");
	return entry::runApp(&app, _argc, _argv);
}

