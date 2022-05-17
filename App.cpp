#include "App.h"
#include "imgui.h"
#include <math.h>
#include <string>
#include <cstdint>
#include <iostream>

namespace MyApp
{
    // docspace
	void RenderUI()
	{
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_AutoHideTabBar;
        static bool no_resize = false;


        ImGui::SetNextWindowPos(ImVec2(0, 0));

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiDockNodeFlags_AutoHideTabBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", nullptr, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        ImGui::Begin("K to RGB Converter App by Odin", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar);

        ImGui::Text("K to RGB Convertion");
        ImGui::Text("White Point set to 6600 K");
        ImGui::Text("While it will work for temperatures outside of the Min/Max,");
        ImGui::Text("estimation quality will decline.");

        // temperature slider
        static float Temperature = 6600.0f; // white point
        static float MaxTemp = 10000;
        static float MinTemp = 1000;
        static float KTemperature;
        const float   f32_zero = MinTemp, f32_one = MaxTemp, f32_lo_a = -10000000000.0f, f32_hi_a = +10000000000.0f;
        
        ImGui::SliderFloat("Max Temperature", &MaxTemp, 1000.0f, 1000000.0f);
        ImGui::SliderFloat("Min Temperature", &MinTemp, 0.0f, 40000.0f);
        ImGui::DragScalar("Temperature", ImGuiDataType_Float, &Temperature, 50.0f, &f32_zero, &f32_one, "%f K"); 
        if (Temperature <= MinTemp) Temperature = MinTemp; // min/max clamps
        if (Temperature >= MaxTemp) Temperature = MaxTemp;

        KTemperature = Temperature / 100.0f;

        static float Red, Blue, Green;

        // K to RGB math
        // thanks to - https://tannerhelland.com/2012/09/18/convert-temperature-rgb-algorithm-code.html
        // red
        if (KTemperature <= 66)
        {
            Red = 255;
        }
        else
        {
            Red = 329.698727446 * pow(KTemperature - 60, -0.1332047592);
        }

        if (Red < 0)
        {
            Red = 0;
        }

        if (Red > 255)
        {
            Red = 255;
        }

        // green
        if (KTemperature <= 66)
        {
            Green = 99.4708025861 * log(KTemperature) - 161.1195681661;
        }
        else
        {
            Green = 288.1221695283 * pow(KTemperature - 60, -0.0755148492);
        }

        if (Green < 0)
        {
            Green = 0;
        }

        if (Green > 255)
        {
            Green = 255;
        }

        // blue
        if (KTemperature >= 66)
        {
            Blue = 255;
        }
        else
        {
            if (KTemperature <= 19)
            {
                Blue = 0;
            }

            else
            {
                Blue = 138.5177312231 * log(KTemperature - 10) - 305.0447927307;
            }

        }

        if (Blue < 0)
        {
            Blue = 0;
        }

        if (Blue > 255)
        {
            Blue = 255;
        }

        // color preview
        static float color2[3]; color2[0] = Red / 255.0f; color2[1] = Green / 255.0f; color2[2] = Blue / 255.0f;
        ImGui::ColorEdit3("", color2);

        static float color1[4]; color1[0] = Red / 255.0f; color1[1] = Green / 255.0f; color1[2] = Blue / 255.0f; color1[3] = 1.0f;
        static bool alpha_preview = true;
        static bool alpha_half_preview = false;
        static bool drag_and_drop = true;
        static bool options_menu = true;
        static bool hdr = false;
        ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
        static bool alpha = true;
        static bool alpha_bar = true;
        static bool side_preview = true;
        static bool ref_color = false;
        static ImVec4 ref_color_v(1.0f, 0.0f, 1.0f, 0.5f);
        static int display_mode = 0;
        static int picker_mode = 0;
        if (side_preview)
        {

        }

        //ImGui::Combo("Display Mode", &display_mode, "Auto/Current\0None\0RGB Only\0HSV Only\0Hex Only\0");
        ImGuiColorEditFlags flags = misc_flags;
        if (!alpha)            flags |= ImGuiColorEditFlags_NoAlpha;        // This is by default if you call ColorPicker3() instead of ColorPicker4()
        if (alpha_bar)         flags |= ImGuiColorEditFlags_AlphaBar;
        if (!side_preview)     flags |= ImGuiColorEditFlags_NoSidePreview;
        if (picker_mode == 1)  flags |= ImGuiColorEditFlags_PickerHueBar;
        if (picker_mode == 2)  flags |= ImGuiColorEditFlags_PickerHueWheel;
        if (display_mode == 1) flags |= ImGuiColorEditFlags_NoInputs;       // Disable all RGB/HSV/Hex displays
        if (display_mode == 2) flags |= ImGuiColorEditFlags_DisplayRGB;     // Override display mode
        if (display_mode == 3) flags |= ImGuiColorEditFlags_DisplayHSV;
        if (display_mode == 4) flags |= ImGuiColorEditFlags_DisplayHex;
        ImGui::ColorPicker4("RGB Color", (float*)&color1, flags, ref_color ? &ref_color_v.x : NULL);

        // converstion to int and printout value
        int RedInt = (int)Red;
        int GreenInt = (int)Green;
        int BlueInt = (int)Blue;

        if (ImGui::CollapsingHeader("Extra Data"))
        {
            ImGui::Text("Red Float = %f", Red);
            ImGui::Text("Green Float = %f", Green);
            ImGui::Text("Blue Float = %f", Blue);
        }

        if (ImGui::CollapsingHeader("Credit"))
        {
            ImGui::Text("https://tannerhelland.com/2012/09/18/convert-temperature-rgb-algorithm-code.html");
        }
        
        ImGui::End();

        ImGui::End();
    }

}
    
