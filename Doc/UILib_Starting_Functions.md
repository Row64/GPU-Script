# **UILib Functions**

A list of available UILib functions and what they do...

## **Navigation**
[Drawing Core](#Drawing-Core) <br>
[Dynamic Widgets](#Dynamic-Widgets) <br>
[Events](#Events) <br>
[Helper Functions](#Helper-Functions) <br>
[ImGui Widgets Remade](#ImGui-Widgets-Remade) <br>

## **Drawing Core**

| Function | Description | Declaration |
|----------|-------------|-------------|
|UIAddTriangle|Adds a triangle with anti-aliasing|`void UIAddTriangle(float inX, float inY, float inRadius, bool inOpen, unsigned int inC);`|
|UIAddFilledPolyline|Filled polyline|`void UIAddFilledPolyline(vector<vec2> inPnts, unsigned int inC );`|
|UIAddPolyline|Add polyline outline|`void UIAddPolyline(vector<vec2> inPnts, unsigned int inC, float inThickness, bool inClosed );`|
|UIAddRoundCornerRect|Rectangle where you specify which corners are rounded|`void UIAddRoundCornerRect(float inX, float inY, float inW, float inH, unsigned int inC, bool inFill, float inThickness, float inRounding, std::string cornerBits);`|
|UIAddCircle|Add circle|`void UIAddCircle(float inX, float inY, float inRadius, unsigned int inC, bool inFill, float inThickness, int nbSegments);`|
|UIAddRect|Add Rectangle - filled or outline|`void UIAddRect(float inX, float inY, float inW, float inH, unsigned int inC, bool inFill, float inThickness);`|
|UIAddRoundRect|Rounded corner rect with all 4 corners rounded|`void UIAddRoundRect(float inX, float inY, float inW, float inH, unsigned int inC, bool inFill, float inThickness, float inRounding);`|
|UIAddLine|Line from point A to point B|`void UIAddLine(float inX1, float inY1, float inX2, float inY2, unsigned int inC1, float inThickness);`|
|UIAddArrowLine|Line from point A to point B with arrowhead|`void UIAddArrowLine(float inX1, float inY1, float inX2, float inY2, unsigned int inC1, float inThickness, float inW_Arrow, float inH_Arrow);`|
|UIAddGradientRect|Rectangle with 2 color gradient|`void UIAddGradientRect(float inX, float inY, float inW, float inH, unsigned int inC1, unsigned int inC2, bool inVertical);`|
|UIAddGradientFade|Rectangle with 2 color gradient, second color fades to transparent|`void UIAddGradientFade(float inX, float inY, float inW, float inH, unsigned int inC, bool inVertical);`|
|UIAddBorderRect|Rectangle with both fill and outline|`void UIAddBorderRect(float inX, float inY, float inW, float inH, unsigned int inFillC, unsigned int inBorderC, float inThickness);`|
|UIPlaceRect|Rectangle placed on point - choose alignment [left, mid, right] on x,y |`void UIPlaceRect(float inX, float inY, ImRect inRect, int inXType, int inYType, unsigned int inC);`|
|UIDrawBB|Draw rectangle filling bounding box |`void UIDrawBB(ImRect inRect, unsigned int inC);`|
|UIDrawAlphaBB|Draw rectangle filling bounding box & set transparency|`void UIDrawAlphaBB(ImRect inRect, unsigned int inC, float inAlpha);`|
|UIDrawBBOutline|Draw rectangle outline on bounding box |`void UIDrawBBOutline(ImRect inRect, unsigned int inC, float inThickness);`|
|UIDrawRoundBB|Draw rounded rectangle filling bounding box |`void UIDrawRoundBB(ImRect inRect, unsigned int inC, float inRounding);`|
|UIAddTextFloat|Draw Text From Number|`void UIAddTextFloat(float inX, float inY, float inW, float inH, float *inVal, vec2 inAlign);`|
|UIAddTextString|Draw Text String|`void UIAddTextString(float inX, float inY, float inW, float inH, string inText, int inC, vec2 inAlign);`|
|UIAddTextWithFont|Draw Text String with specific font|`void UIAddTextWithFont(float inX, float inY, float inW, float inH, string inText, int inC, int inFontIndex, vec2 inAlign);`|




## **Dynamic Widgets**
| Function | Description | Declaration |
|----------|-------------|-------------|
|UIAddSliderH|Horizontal Slider|`void UIAddSliderH(float inX, float inY, float inW, float inH, float* inVal, float inMin, float inMax);`|
|UIAddSliderV|Vertical Slider|`void UIAddSliderV(float inX, float inY, float inW, float inH, float* inVal, float inMin, float inMax);`|
|UIAddScrollerH|Horizontal Scroll Bar|`void UIAddScrollerH(float inX, float inY, float inW, float inH, float inContentW, float* inVal);`|
|UIAddScrollerV|Vertical Scroll Bar|`void UIAddScrollerV(float inX, float inY, float inW, float inH, float inContentH, float* inVal, int* inState);`|
|UIAddTabs|Horizontal Tab Bar|`UIAddTabs(float inX, float inY, float inHeight, vector<string> inStrings, int* inIndex, float inPad, int inFontIndex);`|



## **Events**
| Function | Description | Declaration |
|----------|-------------|-------------|
|GetOverState|True if mouse in bounding box|`bool GetOverState(ImRect inBB);`|
|GetActiveState|True if give ID is active|`bool GetActiveState(bool inOver, ImGuiID inId);`|
|MouseDown|True if mouse down|`bool MouseDown();`|
|DragStart|True is drag started. Only true for 1 frame of drawing|`bool DragStart();`|
|GetMousePos|Global mouse position|`vec2 GetMousePos();`|
|GetLocalMousePos|Local mouse position|`vec2 GetLocalMousePos();`|
|GetKeyDown|Key code of down key|`int GetKeyDown();`|
|GetMouseScroll|Scroll delta from last frame|`float GetMouseScroll();`|



## **Helper Functions**
| Function | Description | Declaration |
|----------|-------------|-------------|
|Cl|Converts Hex Color To RGB, ex: 0xFF0000 becomes {255,0,0,255}|`ImU32 Cl(unsigned int hexValue);`|
|ClearCl|Converts Hex To Transparent Color, ex: 0xFF0000 becomes {255,0,0,0}|`ImU32 ClearCl(unsigned int hexValue);`|
|ClWithAlpha|Converts Hex To Color setting 0-1 alpha, ex: 0xFF0000, .5 becomes {255,0,0,128}|`ImU32 ClWithAlpha(unsigned int hexValue, float inAlpha);`|
|Pos|Convert Global Position to Window Local Space|`ImVec2 Pos(float x, float y, ImVec2 winPos);`|
|LogBB|Log bounding box to terminal|`void LogBB(ImRect inBB);`|
|LogPos|Log 2D position to terminal|`void LogPos(string inTxt, vec2 inPos);`|
|ClipFloat|Clip float number between min & max, useful for scrollers|`float ClipFloat(float n, float lower, float upper);`|
|GetGlobalBB|Given global: (x,y,width,height) return global bounding box |`ImRect GetGlobalBB(float inX, float inY, float inW, float inH);`|
|GetCenteredBB|Given point and width,height return a bounding box centered on that point|`ImRect GetCenteredBB(float inX, float inY, float inW, float inH);`|
|YToGlobal|Given lcoal y value, return global y value|`float YToGlobal(float inY);`|
|GetBB|Given (x,y,width,height) return bounding box|`ImRect GetBB(float inX, float inY, float inW, float inH);`|

