#include "Application.h"

#include "glm/gtc/type_ptr.hpp"

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_dx11.h"
#include "ThirdParty/imgui/imgui_impl_sdl2.h"

#include "Materials/Diffuse.h"
#include "Materials/Emissive.h"

#include "CollidableObjects/Sphere.h"
#include "RayTracedImage.h"
#include "RayTracing/RayEmitter.h"
#include "RayTracing/RayTracer.h"
#include "ScopedTimer.h"
#include "TextureRenderer.h"
#include "Window.h"
#include "World.h"

Application::Application() :
	m_textureRenderer(std::make_unique<TextureRenderer>()),
	m_rayEmitter(std::make_unique<RayEmitter>()),
	m_rayTracer(std::make_unique<RayTracer>()),
	m_rayTracedImage(std::make_unique<RayTracedImage>()),
	m_window(std::make_unique<Window>()),
	m_world(std::make_unique<World>()),

	m_lastMousePosition(-1),
	m_mouseSensitivity(0.002f),

	m_imageViewHovered(false),
	m_generationTime(0.0f),
	m_lastFrameTime(0.0f),
	m_needsResize(false),
	m_initialised(false)
{
}

Application::~Application()
{
	if (m_initialised)
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}
}

bool Application::Initialise()
{
	if (!m_window->Initialise("Ray Tracing Demo"))
		return false;

	if (!m_textureRenderer->Initialise(m_window->GetHWND(), m_window->m_renderWindowRect))
		return false;

	m_rayEmitter->Initialise(m_window->m_renderWindowRect);

	m_rayTracer->Initialise();

	if (!m_rayTracedImage->Initialise(m_window->m_renderWindowRect))
		return false;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// We just need keyborad controls
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;// Enable Multi-Viewport / Platform Windows

	if (!ImGui_ImplSDL2_InitForD3D(m_window->GetInternalWindow()))
	{
		std::cout << "ImGUI win32 init failed" << std::endl;
		return false;
	}
	if (!ImGui_ImplDX11_Init(m_textureRenderer->GetDevice(), m_textureRenderer->GetDeviceContext()))
	{
		std::cout << "ImGui_ImplDX11_Init failed" << std::endl;
		return false;
	}

	InitImGuiStyle();
	m_initialised = true;
	return true;
}

void Application::InitImGuiStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	ImVec4 offWhite = ImVec4(0.969f, 0.965f, 0.949f, 1.0f);
	ImVec4 offWhiteSlightlyTransparent = ImVec4(0.969f, 0.965f, 0.949f, 0.73f);
	ImVec4 offWhiteSemiTransparent = ImVec4(0.969f, 0.965f, 0.949f, 0.58f);

	ImVec4 darkGrayColor = ImVec4(0.827f, 0.824f, 0.804f, 0.92f);
	ImVec4 darkGrayColorSemiTransparent = ImVec4(0.827f, 0.824f, 0.804f, 0.63f);
	ImVec4 textDarkGray = ImVec4(0.412f, 0.412f, 0.412f, 1.00f);

	ImVec4 graySemiTransparent = ImVec4(0.906f, 0.902f, 0.882f, 0.92f);
	ImVec4 grayTransparent = ImVec4(0.906f, 0.902f, 0.882f, 0.78f);

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = textDarkGray;
	style->Colors[ImGuiCol_TextDisabled] = darkGrayColorSemiTransparent;
	style->Colors[ImGuiCol_WindowBg] = graySemiTransparent;
	style->Colors[ImGuiCol_ChildBg] = offWhiteSemiTransparent;
	style->Colors[ImGuiCol_PopupBg] = grayTransparent;
	style->Colors[ImGuiCol_Border] = darkGrayColor;
	style->Colors[ImGuiCol_BorderShadow] = grayTransparent;
	style->Colors[ImGuiCol_FrameBg] = offWhiteSlightlyTransparent;
	style->Colors[ImGuiCol_FrameBgHovered] = darkGrayColorSemiTransparent;
	style->Colors[ImGuiCol_FrameBgActive] = darkGrayColor;
	style->Colors[ImGuiCol_TitleBg] = offWhite;
	style->Colors[ImGuiCol_TitleBgCollapsed] = offWhiteSemiTransparent;
	style->Colors[ImGuiCol_TitleBgActive] = darkGrayColor;
	style->Colors[ImGuiCol_MenuBarBg] = offWhiteSemiTransparent;
	style->Colors[ImGuiCol_ScrollbarBg] = offWhite;
	style->Colors[ImGuiCol_ScrollbarGrab] = darkGrayColorSemiTransparent;
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = grayTransparent;
	style->Colors[ImGuiCol_ScrollbarGrabActive] = darkGrayColor;
	style->Colors[ImGuiCol_CheckMark] = darkGrayColor;
	style->Colors[ImGuiCol_SliderGrab] = darkGrayColorSemiTransparent;
	style->Colors[ImGuiCol_SliderGrabActive] = darkGrayColor;
	style->Colors[ImGuiCol_Button] = darkGrayColorSemiTransparent;
	style->Colors[ImGuiCol_ButtonHovered] = offWhiteSemiTransparent;
	style->Colors[ImGuiCol_ButtonActive] = darkGrayColor;
	style->Colors[ImGuiCol_Header] = darkGrayColorSemiTransparent;
	style->Colors[ImGuiCol_HeaderHovered] = darkGrayColorSemiTransparent;
	style->Colors[ImGuiCol_HeaderActive] = darkGrayColor;
	style->Colors[ImGuiCol_ResizeGrip] = darkGrayColorSemiTransparent;
	style->Colors[ImGuiCol_ResizeGripHovered] = darkGrayColorSemiTransparent;
	style->Colors[ImGuiCol_ResizeGripActive] = darkGrayColor;
	style->Colors[ImGuiCol_PlotLines] = darkGrayColorSemiTransparent;
	style->Colors[ImGuiCol_PlotLinesHovered] = darkGrayColor;
	style->Colors[ImGuiCol_PlotHistogram] = darkGrayColorSemiTransparent;
	style->Colors[ImGuiCol_PlotHistogramHovered] = darkGrayColor;
	style->Colors[ImGuiCol_TextSelectedBg] = darkGrayColorSemiTransparent;
	style->Colors[ImGuiCol_ModalWindowDimBg] = offWhiteSemiTransparent;

	style->Colors[ImGuiCol_Separator] = offWhite;
	style->Colors[ImGuiCol_SeparatorHovered] = offWhite;
	style->Colors[ImGuiCol_SeparatorActive] = offWhite;

	style->Colors[ImGuiCol_Tab] = offWhite;                   // TabItem in a TabBar
	style->Colors[ImGuiCol_TabHovered] = offWhite;
	style->Colors[ImGuiCol_TabActive] = offWhite;
	style->Colors[ImGuiCol_TabUnfocused] = offWhite;
	style->Colors[ImGuiCol_TabUnfocusedActive] = offWhite;
	style->Colors[ImGuiCol_DockingPreview] = offWhite;        // Preview overlay color when about to docking something
	style->Colors[ImGuiCol_DockingEmptyBg] = offWhite;        // Background color for empty node (e.g. CentralNode with no window docked into it)

	style->Colors[ImGuiCol_TableHeaderBg] = offWhite;         // Table header background
	style->Colors[ImGuiCol_TableBorderStrong] = offWhite;     // Table outer and header borders (prefer using Alpha=1.0 here)
	style->Colors[ImGuiCol_TableBorderLight] = offWhite;      // Table inner borders (prefer using Alpha=1.0 here)
	style->Colors[ImGuiCol_TableRowBg] = offWhite;            // Table row background (even rows)
	style->Colors[ImGuiCol_TableRowBgAlt] = offWhite;         // Table row background (odd rows)

	style->Colors[ImGuiCol_DragDropTarget] = offWhite;        // Rectangle highlighting a drop target
	style->Colors[ImGuiCol_NavHighlight] = offWhite;          // Gamepad/keyboard: current highlighted item
	style->Colors[ImGuiCol_NavWindowingHighlight] = offWhite; // Highlight window when using CTRL+TAB
	style->Colors[ImGuiCol_NavWindowingDimBg] = offWhite;     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
}

bool Application::Run()
{
	ScopedTimer timer;

	float deltaTime = glm::min<float>(m_lastFrameTime, 0.0333f);

	if (!Update(deltaTime))
		return false;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGuiID dockspace = ImGui::DockSpaceOverViewport();
	ImGui::SetNextWindowDockID(dockspace);

	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_NoDockingSplit;
	dockspace_flags |= ImGuiDockNodeFlags_AutoHideTabBar | ImGuiDockNodeFlags_NoUndocking;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |  ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDecoration;

	ImGuiWindowFlags viewport_flags = ImGuiWindowFlags_None;
	ImGuiWindowFlags settings_flags = ImGuiWindowFlags_None;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::Begin("DockSpace", nullptr, window_flags);
	
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

		ImGui::Begin("Settings", nullptr, settings_flags);

			ImGui::Text("Last render: %.3fms", m_generationTime);

			bool accumulate = m_rayTracedImage->GetAccumulate();
			if (ImGui::Checkbox("Accumulate", &accumulate))
			{
				m_rayTracedImage->SetAccumulate(accumulate);
				m_rayTracedImage->ResetFrameIndex();
			}

			if (ImGui::Button("Restart Accumulation"))
				m_rayTracedImage->ResetFrameIndex();

			glm::vec3 lightDirection = m_world->GetLightDirection();
			if (ImGui::DragFloat3("Light direction", glm::value_ptr(lightDirection), 0.1f))
			{
				m_world->SetLightDirection(lightDirection);
				m_rayTracedImage->ResetFrameIndex();
			}

			std::vector<CollidableObject*>& spheres = m_world->GetObjectsNonConst();
			bool updated = false;
			for (int i = 0; i < spheres.size(); i++)
			{
				ImGui::PushID(i);

				glm::vec3 spherePosition = spheres[i]->GetPosition();
				if(ImGui::DragFloat3("Position", glm::value_ptr(spherePosition), 0.1f)) {
					spheres[i]->SetPosition(spherePosition);
					updated = true;
				}
				float radius = spheres[i]->GetRadius();
				if (ImGui::DragFloat("Radius", &radius, 0.1f))
				{
					spheres[i]->SetRadius(radius);
					updated = true;
				}

				int materialIndex = spheres[i]->GetMaterialIndex();
				if (ImGui::DragInt("Material", &materialIndex, 1.0f, 0, m_world->GetNumMaterials() - 1))
				{
					spheres[i]->SetMaterialIndex(materialIndex);
					updated = true;
				}

				IMaterial* material = m_world->GetMaterialPtr(spheres[i]->GetMaterialIndex());
				glm::vec3 albedo = material->GetAlbedo();
				// Temporarily disabled tooltip and drag drop due to a bug in ImGui.
				if (ImGui::ColorEdit3("Albedo", glm::value_ptr(albedo), ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoPicker))
				{
					material->SetAlbedo(albedo);
					m_world->UpdateMaterial(material, i);
					updated = true;
				}

				if (material->GetType() == MaterialType::Emissive)
				{
					Emissive* emissiveMaterial = dynamic_cast<Emissive*>(material);
					assert(emissiveMaterial);

					float emissionPower = emissiveMaterial->GetEmissionPower();
					if (ImGui::DragFloat("Emission Power", &emissionPower, 0.05f, 0.0f, FLT_MAX))
					{
						emissiveMaterial->SetEmissionPower(emissionPower);
						m_world->UpdateMaterial(emissiveMaterial, i);
						updated = true;
					}
				}

				if (m_rayTracer && material->GetType() == MaterialType::Diffuse)
				{
					Diffuse* diffuseMaterial = dynamic_cast<Diffuse*>(material);
					assert(diffuseMaterial);

					float roughness = diffuseMaterial->GetRoughness();
					if (ImGui::DragFloat("Roughness", &roughness, 0.05f, 0.0f, 1.0f))
					{
						m_world->UpdateMaterial(material, i);
						updated = true;
					}
				}

				if (updated)
					m_rayTracedImage->ResetFrameIndex();

				ImGui::Separator();

				ImGui::PopID();
			}

		ImGui::End();

		ImGui::Begin("Viewport", nullptr, viewport_flags);

			// Find out if the size of this viewport has changed.
			glm::vec2 contentRegion(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
			if (contentRegion == m_window->m_renderWindowRect)
				m_needsResize = false;
			else
				m_needsResize = true;

			m_window->m_renderWindowRect = contentRegion;

			if (!m_needsResize && m_textureRenderer && m_textureRenderer->GetTextureView())
			{
				m_imageViewHovered = ImGui::IsWindowHovered();

				ImGui::Image((void*)m_textureRenderer->GetTextureView(),
					ImVec2(m_textureRenderer->GetTextureDimensions().x,
						m_textureRenderer->GetTextureDimensions().y));
			}

		ImGui::End();

	ImGui::End();

	if (!Resize())
		return false;

	if (!Render())
		return false;

	m_lastFrameTime = (float)timer.ElapsedTimeInMilliseconds();

	return true;
}

bool Application::Resize()
{
	if (m_needsResize && (m_window->m_renderWindowRect.x != 0.0f) && (m_window->m_renderWindowRect.y != 0.0f))
	{
		m_window->m_mainWindowRect = { m_window->GetWidth(), m_window->GetHeight() };

		if (!m_textureRenderer->Resize(m_window->m_renderWindowRect))
			return false;

		// Number of ray directions is based on the size of the texture image, so this must also be resized.
		m_rayEmitter->Resize(m_window->m_renderWindowRect);
		m_rayTracedImage->Resize(m_window->m_renderWindowRect);
		m_needsResize = false;
	}

	return true;
}

bool Application::Update(float deltaTime)
{
	if (!m_window->HandleEventLoop(deltaTime))
		return false;

	if (m_imageViewHovered)
	{
		if(UpdateFromMouse())
			m_rayTracedImage->ResetFrameIndex();

		if(UpdateFromKeyPress(deltaTime))
			m_rayTracedImage->ResetFrameIndex();
	}

	return true;
}

// Returns true if an update occured
bool Application::UpdateFromMouse()
{
	bool updated = false;

	if (m_imageViewHovered)
	{
		glm::vec2 currentMousePosition = m_window->GetMousePosition();
		if (m_lastMousePosition.x == -1 && m_lastMousePosition.y == -1)
		{
			m_lastMousePosition = currentMousePosition;
			return false;
		}

		if (m_window->IsLeftMouseButtonDown())
		{
			glm::vec2 mouseDelta;
			mouseDelta.x = (currentMousePosition.x - m_lastMousePosition.x) * m_mouseSensitivity;
			mouseDelta.y = (currentMousePosition.y - m_lastMousePosition.y) * m_mouseSensitivity;
			if (mouseDelta.x != 0.0f && mouseDelta.y != 0.0f)
			{
				m_rayEmitter->Rotate(mouseDelta);
				updated = true;
			}
		}
		m_lastMousePosition = currentMousePosition;
	}

	return updated;
}

// Returns true if an update occured
bool Application::UpdateFromKeyPress(float deltaTime)
{
	if (m_window->IsKeyDown(SDLK_DOWN) || m_window->IsKeyDown(SDLK_s))
	{
		m_rayEmitter->MoveBack(deltaTime);
		return true;
	}
	if (m_window->IsKeyDown(SDLK_UP) || m_window->IsKeyDown(SDLK_w))
	{
		m_rayEmitter->MoveForward(deltaTime);
		return true;
	}
	if (m_window->IsKeyDown(SDLK_LEFT) || m_window->IsKeyDown(SDLK_a))
	{
		m_rayEmitter->MoveLeft(deltaTime);
		return true;
	}
	if (m_window->IsKeyDown(SDLK_RIGHT) || m_window->IsKeyDown(SDLK_d))
	{
		m_rayEmitter->MoveRight(deltaTime);
		return true;
	}
	if (m_window->IsKeyDown(SDLK_q))
	{
		m_rayEmitter->MoveDown(deltaTime);
		return true;
	}
	if (m_window->IsKeyDown(SDLK_e))
	{
		m_rayEmitter->MoveUp(deltaTime);
		return true;
	}

	return false;
}

bool Application::Render()
{
	ImGui::Render();

	if (!m_textureRenderer->BeginRender())
		return false;

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ScopedTimer timer;
	uint32_t* pixels = m_rayTracedImage->FillPixels(*m_rayTracer.get(), *m_rayEmitter.get(), *m_world.get());
	if (!pixels)
	{
		std::cout << "failed to create pixel buffer";
		return false;
	}

	m_generationTime = (float)timer.ElapsedTimeInMilliseconds();

	m_textureRenderer->Render(pixels);
	m_textureRenderer->EndRender();

	return true;
}
