#ifndef UILIB_WIDGETS_CPP
#define UILIB_WIDGETS_CPP

#include "UILib.h"

// --------------------------------------------------------------------- //
// [INTERNAL] Copied from imgui_widgets.cpp -- internal helper functions //
// --------------------------------------------------------------------- //

static const ImGuiDataTypeInfo GDataTypeInfo[] =
{
    { sizeof(char),             "%d",   "%d"    },  // ImGuiDataType_S8
    { sizeof(unsigned char),    "%u",   "%u"    },
    { sizeof(short),            "%d",   "%d"    },  // ImGuiDataType_S16
    { sizeof(unsigned short),   "%u",   "%u"    },
    { sizeof(int),              "%d",   "%d"    },  // ImGuiDataType_S32
    { sizeof(unsigned int),     "%u",   "%u"    },
#ifdef _MSC_VER
    { sizeof(ImS64),            "%I64d","%I64d" },  // ImGuiDataType_S64
    { sizeof(ImU64),            "%I64u","%I64u" },
#else
    { sizeof(ImS64),            "%lld", "%lld"  },  // ImGuiDataType_S64
    { sizeof(ImU64),            "%llu", "%llu"  },
#endif
    { sizeof(float),            "%f",   "%f"    },  // ImGuiDataType_Float (float are promoted to double in va_arg)
    { sizeof(double),           "%f",   "%lf"   },  // ImGuiDataType_Double
};
IM_STATIC_ASSERT(IM_ARRAYSIZE(GDataTypeInfo) == ImGuiDataType_COUNT);

// FIXME-LEGACY: Prior to 1.61 our DragInt() function internally used floats and because of this the compile-time default value for format was "%.0f".
// Even though we changed the compile-time default, we expect users to have carried %f around, which would break the display of DragInt() calls.
// To honor backward compatibility we are rewriting the format string, unless IMGUI_DISABLE_OBSOLETE_FUNCTIONS is enabled. What could possibly go wrong?!
static const char* PatchFormatStringFloatToInt(const char* fmt)
{
    if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0) // Fast legacy path for "%.0f" which is expected to be the most common case.
        return "%d";
    const char* fmt_start = ImParseFormatFindStart(fmt);    // Find % (if any, and ignore %%)
    const char* fmt_end = ImParseFormatFindEnd(fmt_start);  // Find end of format specifier, which itself is an exercise of confidence/recklessness (because snprintf is dependent on libc or user).
    if (fmt_end > fmt_start && fmt_end[-1] == 'f')
    {
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
        if (fmt_start == fmt && fmt_end[0] == 0)
            return "%d";
        ImGuiContext& g = *GImGui;
        ImFormatString(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end); // Honor leading and trailing decorations, but lose alignment/precision.
        return g.TempBuffer;
#else
        IM_ASSERT(0 && "DragInt(): Invalid format string!"); // Old versions used a default parameter of "%.0f", please replace with e.g. "%d"
#endif
    }
    return fmt;
};


// --------------------------------------------------------------------- //
// [UILib] Widgets                                                       //
// --------------------------------------------------------------------- //

namespace UILib {

    bool UISliderScalar(const char* label, ImGuiDataType data_type, void* v, const void* v_min, const void* v_max, const char* format, float power)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const float w = CalcItemWidth();

        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
        const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

        ItemSize(total_bb, style.FramePadding.y);
        if (!ItemAdd(total_bb, id, &frame_bb))
            return false;
        
        // Set Colors
        ImVec4 cmCol_SliderFront = ImVec4(0.529f, 0.529f, 0.529f, 1.00f); 
        ImVec4 cmCol_SliderBack = ImVec4(0.839f, 0.839f, 0.839f, 1.00f); 
        ImVec4 cmCol_SliderHovered = ImVec4(0.18f, 0.667f, 0.863f, 1.00f); 
        ImVec4 cmCol_SliderDrag = ImVec4(0.149f, 0.545f, 0.667f, 1.00f);
        
        // Set Thickness
        ImRect draw_bb = frame_bb;
        float sliderThicknessScale = 1.2f;
        float shrink_amount = (float)(int)((frame_bb.Max.y - frame_bb.Min.y) * 0.5f * sliderThicknessScale);
        draw_bb.Min.y += shrink_amount;
        draw_bb.Max.y -= shrink_amount;

        // Default format string when passing NULL
        if (format == NULL)
            format = DataTypeGetInfo(data_type)->PrintFmt;
        else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
            format = PatchFormatStringFloatToInt(format);

        // Tabbing or CTRL-clicking on Slider turns it into an input box
        const bool hovered = ItemHoverable(frame_bb, id);
        bool temp_input_is_active = TempInputTextIsActive(id);
        bool temp_input_start = false;
        if (!temp_input_is_active)
        {
            const bool focus_requested = FocusableItemRegister(window, id);
            const bool clicked = (hovered && g.IO.MouseClicked[0]);
            if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
            {
                SetActiveID(id, window);
                SetFocusID(id, window);
                FocusWindow(window);
                g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
                if (focus_requested || (clicked && g.IO.KeyCtrl) || g.NavInputId == id)
                {
                    temp_input_start = true;
                    FocusableItemUnregister(window);
                }
            }
        }
        if (temp_input_is_active || temp_input_start)
            return TempInputTextScalar(frame_bb, id, label, data_type, v, format);


        // Slider behavior
        ImRect grab_bb;
        const bool value_changed = SliderBehavior(frame_bb, id, data_type, v, v_min, v_max, format, power, ImGuiSliderFlags_None, &grab_bb);
        if (value_changed)
            MarkItemEdited(id);
            

        // Set grab color
        ImU32 circle_col = GetColorU32(cmCol_SliderFront);
        ImU32 frontLine_col = GetColorU32(cmCol_SliderFront);
        ImU32 backLine_col = GetColorU32(cmCol_SliderBack);
        if(g.HoveredId == id){ // hover over slider
            frontLine_col = GetColorU32(cmCol_SliderHovered);
        }
        if( g.ActiveId == id){  // mouse down
            circle_col = GetColorU32(cmCol_SliderDrag);
        }
        else if(g.HoveredId == id){ // hover over slider
            circle_col = GetColorU32(cmCol_SliderHovered);
        }

        // Render grab
        ImVec2 middle(((grab_bb.Max.x + grab_bb.Min.x)/2.0f),((grab_bb.Max.y + grab_bb.Min.y)/2.0f));
        
        draw_bb.Min.x += 2;
        draw_bb.Max.x -= 2;
        draw_bb.Min.y = middle.y - 1.5f;
        draw_bb.Max.y = middle.y + 1.5f;

        float frameRounding = 0.0f;
        window->DrawList->AddRectFilled(draw_bb.Min, ImVec2(middle.x, draw_bb.Max.y), frontLine_col, frameRounding, ImDrawCornerFlags_Left);
        window->DrawList->AddRectFilled(ImVec2(middle.x, draw_bb.Min.y), draw_bb.Max, backLine_col, frameRounding, ImDrawCornerFlags_Right);

        
        // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
        char value_buf[64];
        const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, v, format);
        

        RenderText(ImVec2(frame_bb.Min.x +9 ,frame_bb.Min.y - 7), value_buf, value_buf_end, NULL);

        float circle_rad = 4.5f;

        window->DrawList->AddCircleFilled(ImVec2(grab_bb.Min.x + 5, grab_bb.Min.y + ((grab_bb.Max.y - grab_bb.Min.y) / 2)), circle_rad, circle_col, 16);

        if (label_size.x > 0.0f)
            RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
        return value_changed;
    }

    // Add multiple sliders on 1 line for compact edition of multiple components
    bool UISliderScalarN(const char* label, ImGuiDataType data_type, void* v, int components, const void* v_min, const void* v_max, const char* format, float power)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        bool value_changed = false;
        BeginGroup();
        PushID(label);
        PushMultiItemsWidths(components, CalcItemWidth());
        size_t type_size = GDataTypeInfo[data_type].Size;
        for (int i = 0; i < components; i++)
        {
            PushID(i);
            if (i > 0)
                SameLine(0, g.Style.ItemInnerSpacing.x);
            value_changed |= UISliderScalar("", data_type, v, v_min, v_max, format, power);
            PopID();
            PopItemWidth();
            v = (void*)((char*)v + type_size);
        }
        PopID();

        const char* label_end = FindRenderedTextEnd(label);
        if (label != label_end)
        {
            SameLine(0, g.Style.ItemInnerSpacing.x);
            TextEx(label, label_end);
        }

        EndGroup();
        return value_changed;
    }

    bool UISliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, float power)
    {
        return UISliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, power);
    }

    bool UISliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format, float power)
    {
        return UISliderScalarN(label, ImGuiDataType_Float, v, 2, &v_min, &v_max, format, power);
    }

    bool UISliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format, float power)
    {
        return UISliderScalarN(label, ImGuiDataType_Float, v, 3, &v_min, &v_max, format, power);
    }

    bool UISliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format, float power)
    {
        return UISliderScalarN(label, ImGuiDataType_Float, v, 4, &v_min, &v_max, format, power);
    }

    bool UISliderAngle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max, const char* format)
    {
        if (format == NULL)
            format = "%.0f deg";
        float v_deg = (*v_rad) * 360.0f / (2*IM_PI);
        bool value_changed = UISliderFloat(label, &v_deg, v_degrees_min, v_degrees_max, format, 1.0f);
        *v_rad = v_deg * (2*IM_PI) / 360.0f;
        return value_changed;
    }

    bool UISliderInt(const char* label, int* v, int v_min, int v_max, const char* format)
    {
        return UISliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format);
    }

    bool UISliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format)
    {
        return UISliderScalarN(label, ImGuiDataType_S32, v, 2, &v_min, &v_max, format);
    }

    bool UISliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format)
    {
        return UISliderScalarN(label, ImGuiDataType_S32, v, 3, &v_min, &v_max, format);
    }

    bool UISliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format)
    {
        return UISliderScalarN(label, ImGuiDataType_S32, v, 4, &v_min, &v_max, format);
    }

    bool UIVSliderScalar(const char* label, const ImVec2& size, ImGuiDataType data_type, void* v, const void* v_min, const void* v_max, const char* format, float power)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + size);
        const ImRect bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(frame_bb, id))
            return false;

        // Set colors
        ImVec4 cmCol_SliderFront = ImVec4(0.529f, 0.529f, 0.529f, 1.00f); 
        ImVec4 cmCol_SliderBack = ImVec4(0.839f, 0.839f, 0.839f, 1.00f); 
        ImVec4 cmCol_SliderHovered = ImVec4(0.18f, 0.667f, 0.863f, 1.00f); 
        ImVec4 cmCol_SliderDrag = ImVec4(0.149f, 0.545f, 0.667f, 1.00f);
        
        // Set thickness
        ImRect draw_bb = frame_bb;
        float sliderThicknessScale = 1.2f;
        float shrink_amount = (float)(int)((frame_bb.Max.x - frame_bb.Min.x) * 0.5f * sliderThicknessScale);
        draw_bb.Min.x += shrink_amount;
        draw_bb.Max.x -= shrink_amount;
        
        // Default format string when passing NULL
        if (format == NULL)
            format = DataTypeGetInfo(data_type)->PrintFmt;
        else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
            format = PatchFormatStringFloatToInt(format);

        const bool hovered = ItemHoverable(frame_bb, id);
        if ((hovered && g.IO.MouseClicked[0]) || g.NavActivateId == id || g.NavInputId == id)
        {
            SetActiveID(id, window);
            SetFocusID(id, window);
            FocusWindow(window);
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Up) | (1 << ImGuiDir_Down);
        }


        // Slider behavior
        ImRect grab_bb;
        const bool value_changed = SliderBehavior(frame_bb, id, data_type, v, v_min, v_max, format, power, ImGuiSliderFlags_Vertical, &grab_bb);
        if (value_changed)
            MarkItemEdited(id);


        // Set grab colors
        ImU32 circle_col = GetColorU32(cmCol_SliderFront);
        ImU32 frontLine_col = GetColorU32(cmCol_SliderFront);
        ImU32 backLine_col = GetColorU32(cmCol_SliderBack);
        if(g.HoveredId == id){ // hover over slider
            frontLine_col = GetColorU32(cmCol_SliderHovered);
        }
        if( g.ActiveId == id){  // mouse down
            circle_col = GetColorU32(cmCol_SliderDrag);
        }
        else if(g.HoveredId == id){ // hover over slider
            circle_col = GetColorU32(cmCol_SliderHovered);
        }

        // Render grab
        ImVec2 middle(((grab_bb.Max.x + grab_bb.Min.x)/2.0f),((grab_bb.Max.y + grab_bb.Min.y)/2.0f));

        draw_bb.Min.x = middle.x - 1.5f;
        draw_bb.Max.x = middle.x + 1.5f;
        draw_bb.Min.y += 2;
        draw_bb.Max.y -= 2;

        float frameRounding = 0.0f;
        window->DrawList->AddRectFilled(draw_bb.Min, ImVec2(draw_bb.Max.x, middle.y), frontLine_col, frameRounding, ImDrawCornerFlags_Left);
        window->DrawList->AddRectFilled(ImVec2(draw_bb.Min.x, middle.y), draw_bb.Max, backLine_col, frameRounding, ImDrawCornerFlags_Right);

        // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
        // For the vertical slider we allow centered text to overlap the frame padding
        char value_buf[64];
        const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, v, format);

        RenderText(ImVec2(frame_bb.Min.x - 5 ,frame_bb.Max.y + 7), value_buf, value_buf_end, NULL);

        float circle_rad = 4.5f;

        window->DrawList->AddCircleFilled(ImVec2(grab_bb.Min.x + ((grab_bb.Max.x - grab_bb.Min.x) / 2), grab_bb.Min.y + 5), circle_rad, circle_col, 16);

        //***************** END MOD ******************

        if (label_size.x > 0.0f)
            RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

        return value_changed;
    }

    bool UIVSliderFloat(const char* label, const ImVec2& size, float* v, float v_min, float v_max, const char* format, float power)
    {
        return UIVSliderScalar(label, size, ImGuiDataType_Float, v, &v_min, &v_max, format, power);
    }

    bool UIVSliderInt(const char* label, const ImVec2& size, int* v, int v_min, int v_max, const char* format)
    {
        return UIVSliderScalar(label, size, ImGuiDataType_S32, v, &v_min, &v_max, format);
    }

    
    
} // end namespace UILib

#endif // UILIB_WIDGETS_CPP