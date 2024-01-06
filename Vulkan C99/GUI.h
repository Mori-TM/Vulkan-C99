Vertex GUIQuadVertices;
Index GUIQuadIndices;

unsigned int GUIBufferCount = 1;
VkBuffer* GUIVertexBuffers;
VkBuffer* GUIIndexBuffers;

unsigned int GUITextures[5];

typedef struct
{
	vec2 Pos;
	vec2 Size;
	vec4 Color;

	char* Name;

	unsigned int ID;

	bool FirstInit;
} GUIComponent;

unsigned int GUIWindowCount = 0;
unsigned int GUIComponentCount = 0;
GUIComponent GUIComponents[128];

unsigned int MousePosX = 0;
unsigned int MousePosY = 0;

unsigned int MouseState;

bool GUIFirstInit = true;

bool MouseDown = false;

bool GUIUseCoustumPos = false;
vec3 GUICoustumPos;

//0 = Nothing
//1 = Up
//2 = Down
int GUIMouseWheel = 0;

//GUI Settings
float GUIWindowBoarder = 40;
float GUIFontSize = 20;

vec4 GUIWindowColor = { 0.03, 0.03, 0.03, 1 };

void GUIInit()
{
	GUIVertexBuffers = malloc(GUIBufferCount * sizeof(VkBuffer));
	GUIIndexBuffers = malloc(GUIBufferCount * sizeof(VkBuffer));

	GUIQuadVertices.Size = 6;
	GUIQuadVertices.Vertex = malloc(4 * sizeof(VertexCore));

	GUIQuadVertices.Vertex[0].Pos = Vec3(0.0, 0.0, 0.0);
	GUIQuadVertices.Vertex[1].Pos = Vec3(1.0, 0.0, 0.0);
	GUIQuadVertices.Vertex[2].Pos = Vec3(1.0, 1.0, 0.0);
	GUIQuadVertices.Vertex[3].Pos = Vec3(0.0, 1.0, 0.0);

	GUIQuadVertices.Vertex[0].Color = Vec3(1.0, 1.0, 1.0);
	GUIQuadVertices.Vertex[1].Color = Vec3(1.0, 1.0, 1.0);
	GUIQuadVertices.Vertex[2].Color = Vec3(1.0, 1.0, 1.0);
	GUIQuadVertices.Vertex[3].Color = Vec3(1.0, 1.0, 1.0);

	GUIQuadVertices.Vertex[0].TexCoord = Vec2(0.0, 1.0);
	GUIQuadVertices.Vertex[1].TexCoord = Vec2(1.0, 1.0);
	GUIQuadVertices.Vertex[2].TexCoord = Vec2(1.0, 0.0);
	GUIQuadVertices.Vertex[3].TexCoord = Vec2(0.0, 0.0);

	GUIQuadIndices.Size = 6;
	GUIQuadIndices.Index = malloc(6 * sizeof(int));
	GUIQuadIndices.Index[0] = 0;
	GUIQuadIndices.Index[1] = 1;
	GUIQuadIndices.Index[2] = 2;

	GUIQuadIndices.Index[3] = 2;
	GUIQuadIndices.Index[4] = 3;
	GUIQuadIndices.Index[5] = 0;


	if (CreateVertexBuffer(&GUIVertexBuffers[0], &GUIQuadVertices) != VK_SUCCESS)
		printf("Failed to Create Vertex Buffer\n");

	if (CreateIndexBuffer(&GUIIndexBuffers[0], &GUIQuadIndices) != VK_SUCCESS)
		printf("Failed to Create Index Buffer\n");
}

void GUIDeInit()
{
	vkDestroyBuffer(Device, GUIVertexBuffers[0], NULL);
	vkDestroyBuffer(Device, GUIIndexBuffers[0], NULL);
}

void GUITexturedWindow(vec2 Pos, vec2 Size, int Type)
{
	VertexConst.Model = LoadMat4Identity();
	VertexConst.Model = ScaleMat4(VertexConst.Model, Size.x, Size.y, 1.0);
	VertexConst.Model = TranslateMat4(VertexConst.Model, Pos.x, Pos.y, 0.0);

	GUIFragmentConst.Display = Type;
	GUIFragmentConst.Color = Vec4(0, 0, 0, 0);
	vkCmdPushConstants(CommandBuffers[ImageIndex], GUIPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, FRAGMENT_OFFSET, sizeof(GUIFragmentPushConstant), &GUIFragmentConst);

	vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, GUIPipelineLayout, 0, 1, &TextureDescriptorSets[0], 0, NULL);
	DrawIndices(&GUIQuadIndices, &GUIPipelineLayout);
}

void GUIBindChar()
{
	vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, GUIPipelineLayout, 0, 1, &TextureDescriptorSets[GUITextures[4]], 0, NULL);
}

void GUIDrawChar(vec2 Pos, vec2 Size, char Char)
{
	VertexConst.Model = LoadMat4Identity();
	VertexConst.Model = ScaleMat4(VertexConst.Model, Size.x, Size.y, 1.0);
	VertexConst.Model = TranslateMat4(VertexConst.Model, Pos.x, Pos.y, 0.8);

	GUIFragmentConst.Display = 8;

	// n
	// yyyyxxxx
	// >> 4
	// Char = '!';
	unsigned char n = Char & 0xff;

	float OffsetX = n & 0x0f;
	float OffsetY = n >> 4;

	GUIFragmentConst.Color = Vec4(OffsetX * 0.0625, OffsetY * 0.0625, 1, 1);

	vkCmdPushConstants(CommandBuffers[ImageIndex], GUIPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, FRAGMENT_OFFSET, sizeof(GUIFragmentPushConstant), &GUIFragmentConst);
	DrawIndices(&GUIQuadIndices, &GUIPipelineLayout);
}

void GUIDrawText(vec2 Pos, vec2 Size, const char* Text)
{
	GUIBindChar();

	unsigned int j = 0;

	for (unsigned int i = 0; i < strlen(Text); i++)
	{
		if (Text[i] == '\n')
		{
			Pos.y -= Size.y;
			j = 0;
		}
		else if (Text[i] == '\t')
		{
			j += 4;
		}
		else
		{
			GUIDrawChar(Vec2(Pos.x + (j * (Size.x * 0.7)), Pos.y), Size, Text[i]);
			j++;
		}
	}
}

void GUIBeginWindow(const char* Name, vec2 Pos, vec2 Size)
{
	VertexConst.Model = LoadMat4Identity();
	VertexConst.Model = ScaleMat4(VertexConst.Model, Size.x, Size.y, 1.0);
	VertexConst.Model = TranslateMat4(VertexConst.Model, Pos.x, Pos.y, 0.0);

	GUIFragmentConst.Display = 2;
	GUIFragmentConst.Color = GUIWindowColor;
	vkCmdPushConstants(CommandBuffers[ImageIndex], GUIPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, FRAGMENT_OFFSET, sizeof(GUIFragmentPushConstant), &GUIFragmentConst);

	vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, GUIPipelineLayout, 0, 1, &TextureDescriptorSets[0], 0, NULL);
	DrawIndices(&GUIQuadIndices, &GUIPipelineLayout);

	GUIComponents[GUIWindowCount].Pos = Pos;
	GUIComponents[GUIWindowCount].Size = Size;
	GUIComponents[GUIWindowCount].Color = GUIFragmentConst.Color;
	GUIComponents[GUIWindowCount].Name = Name;
	GUIComponents[GUIWindowCount].ID = GUIWindowCount;
	
	GUIComponentCount++;
}

void GUIEndWindow()
{
//	vec2 Size = { 20, GUIComponents[GUIWindowCount].Size.y };
//	VertexConst.Model = LoadMat4Identity();
//	VertexConst.Model = ScaleMat4(VertexConst.Model, Size.x, Size.y, 1.0);
//	VertexConst.Model = TranslateMat4(VertexConst.Model, GUIComponents[GUIWindowCount].Pos.x + GUIComponents[GUIWindowCount].Size.x - Size.x, 0.0, 0.9);
//
//	GUIFragmentConst.Display = 2;
//	GUIFragmentConst.Color = Add4(GUIWindowColor, Vec4(0.03, 0.03, 0.03, 1));
//	vkCmdPushConstants(CommandBuffers[ImageIndex], GUIPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, FRAGMENT_OFFSET, sizeof(GUIFragmentPushConstant), &GUIFragmentConst);
//
//	vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, GUIPipelineLayout, 0, 1, &TextureDescriptorSets[0], 0, NULL);
//	DrawIndices(&GUIQuadIndices, &GUIPipelineLayout);
//
//	vec2 ScrollbarSize = { Size.x * 0.5, GUIComponents[GUIWindowCount].Size.y * (GUIComponentCount * 0.1) };
//	VertexConst.Model = LoadMat4Identity();
//	VertexConst.Model = ScaleMat4(VertexConst.Model, ScrollbarSize.x, ScrollbarSize.y, 1.0);
//	VertexConst.Model = TranslateMat4(VertexConst.Model, GUIComponents[GUIWindowCount].Pos.x + GUIComponents[GUIWindowCount].Size.x - ScrollbarSize.x * 1.5, 0.0, 1.0);
//
//	GUIFragmentConst.Display = 2;
//	GUIFragmentConst.Color = Add4(GUIWindowColor, Vec4(0.06, 0.06, 0.06, 1));
//	vkCmdPushConstants(CommandBuffers[ImageIndex], GUIPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, FRAGMENT_OFFSET, sizeof(GUIFragmentPushConstant), &GUIFragmentConst);
//
//	vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, GUIPipelineLayout, 0, 1, &TextureDescriptorSets[0], 0, NULL);
//	DrawIndices(&GUIQuadIndices, &GUIPipelineLayout);

//	if(GUIMouseWheel == 1)
//		for(int i = 1; i < GUIComponentCount; i++)
//			GUIComponents[i].Pos.y +

	GUIWindowCount += GUIComponentCount;
}

bool GUIButton(const char* Label)
{
	vec2 ButtonSize = { 0, GUIFontSize * 1.5 };
	vec3 ButtonPos;

	if (!GUIUseCoustumPos)
	{
		ButtonPos = Vec3
		(
			GUIComponents[GUIWindowCount].Pos.x + GUIWindowBoarder,
			GUIComponentCount == 1 ? GUIComponents[GUIComponentCount - 1].Pos.y + GUIWindowBoarder
			: 
			GUIComponents[GUIComponentCount - 1].Pos.y + GUIComponents[GUIComponentCount - 1].Size.y + 10,
			0.5
		);
	}
	else
		ButtonPos = GUICoustumPos;

	
	ButtonSize.x = ((strlen(Label) * 0.7) * (GUIFontSize)) * 1.1;
//	ButtonSize.x = (strlen(Label) * (GUIFontSize * 0.7)) + 10;

	GUIComponents[GUIComponentCount].Pos = Vec2(ButtonPos.x, ButtonPos.y);
	GUIComponents[GUIComponentCount].Size = ButtonSize;
	GUIComponents[GUIComponentCount].Color = GUIFragmentConst.Color;
	GUIComponents[GUIComponentCount].Name = Label;
	GUIComponents[GUIComponentCount].ID = GUIWindowCount;

	

	VertexConst.Model = LoadMat4Identity();
	VertexConst.Model = ScaleMat4(VertexConst.Model, ButtonSize.x, ButtonSize.y, 1.0);
	VertexConst.Model = TranslateMat4V(VertexConst.Model, ButtonPos);

	GUIFragmentConst.Display = 1;

	bool Selected = MousePosX >= ButtonPos.x && 
					MousePosX <= ButtonPos.x + ButtonSize.x && 
					OffScreenPass[1].Height - MousePosY >= ButtonPos.y && 
					OffScreenPass[1].Height - MousePosY <= ButtonPos.y + ButtonSize.y;

	bool ReturnValue = false;

	GUIFragmentConst.Color = Vec4(0.06, 0.06, 0.06, 1.0);

	if (Selected)
		GUIFragmentConst.Color = Vec4(0.0, 0.1484375, 0.7421875, 1);



	if (Selected && MouseDown)
	{
		GUIFragmentConst.Color = Vec4(0.0, 0.1, 0.7, 1);
		ReturnValue = true;
		MouseDown = false;
	}

	vkCmdPushConstants(CommandBuffers[ImageIndex], GUIPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, FRAGMENT_OFFSET, sizeof(GUIFragmentPushConstant), &GUIFragmentConst);

	vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, GUIPipelineLayout, 0, 1, &TextureDescriptorSets[GUITextures[3]], 0, NULL);
	DrawIndices(&GUIQuadIndices, &GUIPipelineLayout);

	GUIDrawText(Vec2(ButtonPos.x + (GUIFontSize * 0.25), ButtonPos.y + (GUIFontSize * 0.25)), Vec2(GUIFontSize, GUIFontSize), Label);

	GUIComponentCount++;
	GUIUseCoustumPos = false;

	return ReturnValue;
}

void GUISlider(const char* Label, float* Value, float Min, float Max)
{
	static vec3 SliderPos;
	vec2 SliderSize = { 200, 5 };

	static vec3 ButtonPos;
	vec2 ButtonSize = { 30,	30 };

	if (!GUIUseCoustumPos)
	{
		SliderPos = Vec3
		(
			(GUIComponents[GUIWindowCount].Pos.x + GUIWindowBoarder + ButtonSize.x * 0.5),
			GUIComponentCount == 1 ? GUIComponents[GUIComponentCount - 1].Pos.y + GUIWindowBoarder
			:
			(GUIComponents[GUIComponentCount - 1].Pos.y + GUIComponents[GUIComponentCount - 1].Size.y + ButtonSize.x) - SliderSize.y * 0.5,
			0.5
		);
	}
	else
		SliderPos = GUICoustumPos;

	GUIComponents[GUIComponentCount].Pos = Vec2(SliderPos.x, SliderPos.y);
	GUIComponents[GUIComponentCount].Size = Vec2(ButtonSize.x, ButtonSize.y - 10);
	GUIComponents[GUIComponentCount].Color = GUIFragmentConst.Color;
	GUIComponents[GUIComponentCount].Name = Label;
	GUIComponents[GUIComponentCount].ID = GUIWindowCount;

	GUIFragmentConst.Display = 2;

	VertexConst.Model = LoadMat4Identity();
	VertexConst.Model = ScaleMat4(VertexConst.Model, SliderSize.x, SliderSize.y, 1.0);
	VertexConst.Model = TranslateMat4V(VertexConst.Model, SliderPos);

	GUIFragmentConst.Color = Vec4(0.06, 0.06, 0.06, 1.0);
	vkCmdPushConstants(CommandBuffers[ImageIndex], GUIPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, FRAGMENT_OFFSET, sizeof(GUIFragmentPushConstant), &GUIFragmentConst);

	vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, GUIPipelineLayout, 0, 1, &TextureDescriptorSets[0], 0, NULL);
	DrawIndices(&GUIQuadIndices, &GUIPipelineLayout);

	//**Button**//
	ButtonPos.y = (SliderPos.y - ButtonSize.y * 0.5) + SliderSize.y * 0.5;
	ButtonPos.z = 0.25;

	vec2 MXY = { MousePosX, OffScreenPass[1].Height - MousePosY };
	vec2 AddVal = Mul2(ButtonSize, Vec2(0.5, 0.5));
	
	//Calculate Button Pos from input value
	{
		float min = GUIComponents[GUIWindowCount].Pos.x + GUIWindowBoarder;
		float max = GUIComponents[GUIWindowCount].Pos.x + SliderSize.x + GUIWindowBoarder;

		float ConvPos = (-((Min - *Value) / (Max - Min)));
		
		float Val = min + ConvPos * SliderSize.x;

		ButtonPos.x = Val;
	}
	
	if (GUIFirstInit)
	{
		if (ButtonPos.x <= GUIComponents[GUIWindowCount].Pos.x + GUIWindowBoarder)
			*Value = Min;

		if (ButtonPos.x >= GUIComponents[GUIWindowCount].Pos.x + SliderSize.x + GUIWindowBoarder)
			*Value = Max;
	}

	bool Selected = (GetDistanceBetween2Points2(MXY, Add2(Vec2(ButtonPos.x, ButtonPos.y), AddVal)) < 19 ? true : false);

	if (Selected)
		GUIFragmentConst.Color = Vec4(0.0, 0.1484375, 0.7421875, 1);

	if (Selected && MouseDown)
	{
		GUIFragmentConst.Color = Vec4(0.0, 0.1, 0.7, 1);
		float Pos = ((float)MousePosX - ButtonSize.x * 0.5);
		ButtonPos.x = Pos;

		if (ButtonPos.x <= GUIComponents[GUIWindowCount].Pos.x + GUIWindowBoarder)
			ButtonPos.x = GUIComponents[GUIWindowCount].Pos.x + GUIWindowBoarder;
	
		if (ButtonPos.x >= GUIComponents[GUIWindowCount].Pos.x + SliderSize.x + GUIWindowBoarder)
			ButtonPos.x = GUIComponents[GUIWindowCount].Pos.x + SliderSize.x + GUIWindowBoarder;

	
		float ConvPos = (-(((GUIComponents[GUIWindowCount].Pos.x + GUIWindowBoarder) - ButtonPos.x) / SliderSize.x));

		*Value = Min + ConvPos * (Max + fabsf(Min));
	}

	VertexConst.Model = LoadMat4Identity();
	VertexConst.Model = ScaleMat4(VertexConst.Model, ButtonSize.x, ButtonSize.y, 1.0);
	VertexConst.Model = TranslateMat4V(VertexConst.Model, ButtonPos);

	GUIFragmentConst.Display = 1;
	vkCmdPushConstants(CommandBuffers[ImageIndex], GUIPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, FRAGMENT_OFFSET, sizeof(GUIFragmentPushConstant), &GUIFragmentConst);

	vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, GUIPipelineLayout, 0, 1, &TextureDescriptorSets[GUITextures[2]], 0, NULL);
	DrawIndices(&GUIQuadIndices, &GUIPipelineLayout);

	GUIDrawText
	(
		Vec2(SliderPos.x + SliderSize.x + ButtonSize.x * 0.5, (SliderPos.y + SliderSize.y * 0.5) - GUIFontSize * 0.5),
		Vec2(GUIFontSize, GUIFontSize), 
		Label
	);

	GUIComponentCount++;
	GUIUseCoustumPos = false;
}

bool GUICheckBox(const char* Label, bool* Value)
{
	vec3 CheckBoxPos;
	vec2 CheckBoxSize = { GUIFontSize * 1.75, GUIFontSize * 1.75 };

	if (!GUIUseCoustumPos)
	{
		CheckBoxPos = Vec3
		(
			GUIComponents[GUIWindowCount].Pos.x + GUIWindowBoarder,
			GUIComponentCount == 1 ? GUIComponents[GUIComponentCount - 1].Pos.y + GUIWindowBoarder
			:
			GUIComponents[GUIComponentCount - 1].Pos.y + GUIComponents[GUIComponentCount - 1].Size.y + 10,
			0.5
		);
	}
	else
		CheckBoxPos = GUICoustumPos;

	GUIComponents[GUIComponentCount].Pos = Vec2(CheckBoxPos.x, CheckBoxPos.y);
	GUIComponents[GUIComponentCount].Size = CheckBoxSize;
	GUIComponents[GUIComponentCount].Color = GUIFragmentConst.Color;
	GUIComponents[GUIComponentCount].Name = Label;
	GUIComponents[GUIComponentCount].ID = GUIWindowCount;

	VertexConst.Model = LoadMat4Identity();
	VertexConst.Model = ScaleMat4(VertexConst.Model, CheckBoxSize.x, CheckBoxSize.y, 1.0);
	VertexConst.Model = TranslateMat4V(VertexConst.Model, CheckBoxPos);

	GUIFragmentConst.Display = 1;

	bool Selected = MousePosX >= CheckBoxPos.x && MousePosX <= CheckBoxPos.x + CheckBoxSize.x && OffScreenPass[1].Height - MousePosY >= CheckBoxPos.y && OffScreenPass[1].Height - MousePosY <= CheckBoxPos.y + CheckBoxSize.y;

	GUIFragmentConst.Color = Vec4(0.06, 0.06, 0.06, 1.0);

	if (Selected)
		GUIFragmentConst.Color = Vec4(0.0, 0.1484375, 0.7421875, 1);

//	if (Selected && MouseDown && Event.type == SDL_MOUSEBUTTONUP && Event.button.button == SDL_BUTTON_LEFT)
	if (Selected && MouseDown)
	{
		GUIFragmentConst.Color = Vec4(0.0, 0.1, 0.7, 1);
		*Value = !*Value;
		MouseDown = false;
	}


	vkCmdPushConstants(CommandBuffers[ImageIndex], GUIPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, FRAGMENT_OFFSET, sizeof(GUIFragmentPushConstant), &GUIFragmentConst);

	vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, GUIPipelineLayout, 0, 1, &TextureDescriptorSets[*Value == false ? GUITextures[0] : GUITextures[1]], 0, NULL);
	DrawIndices(&GUIQuadIndices, &GUIPipelineLayout);

	GUIDrawText(Vec2(CheckBoxPos.x + CheckBoxSize.x, CheckBoxPos.y + CheckBoxSize.y * 0.25), Vec2(CheckBoxSize.y * 0.5, CheckBoxSize.y * 0.5), Label);

	GUIComponentCount++;
	GUIUseCoustumPos = false;
}

int GUIImageRenderMode = 9;
void GUIImage(const char* Label, unsigned int Texture, vec2 Size)
{
	vec3 ImagePos;

	if (!GUIUseCoustumPos)
	{
		ImagePos = Vec3
		(
			GUIComponents[GUIWindowCount].Pos.x + GUIWindowBoarder,
			GUIComponentCount == 1 ? GUIComponents[GUIComponentCount - 1].Pos.y + GUIWindowBoarder
			:
			GUIComponents[GUIComponentCount - 1].Pos.y + GUIComponents[GUIComponentCount - 1].Size.y + 10,
			0.5
		);
	}
	else
		ImagePos = GUICoustumPos;

	GUIComponents[GUIComponentCount].Pos = Vec2(ImagePos.x, ImagePos.y);
	GUIComponents[GUIComponentCount].Size = Size;
	GUIComponents[GUIComponentCount].Color = GUIFragmentConst.Color;
	GUIComponents[GUIComponentCount].Name = Label;
	GUIComponents[GUIComponentCount].ID = GUIWindowCount;

	VertexConst.Model = LoadMat4Identity();
	VertexConst.Model = ScaleMat4(VertexConst.Model, Size.x, Size.y, 1.0);
	VertexConst.Model = TranslateMat4V(VertexConst.Model, ImagePos);

	GUIFragmentConst.Display = GUIImageRenderMode;

	GUIFragmentConst.Color = Vec4(1, 1, 1, 1);

	vkCmdPushConstants(CommandBuffers[ImageIndex], GUIPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, FRAGMENT_OFFSET, sizeof(GUIFragmentPushConstant), &GUIFragmentConst);

	vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, GUIPipelineLayout, 0, 1, &TextureDescriptorSets[Texture], 0, NULL);
	DrawIndices(&GUIQuadIndices, &GUIPipelineLayout);
	GUIDrawText(Vec2(ImagePos.x + Size.x, ImagePos.y), Vec2(GUIFontSize, GUIFontSize), Label);

	GUIComponentCount++;
	GUIUseCoustumPos = false;
}

void GUIText(const char* Text)
{
	vec3 TextPos;

	int LineFeats = 0;
	for (int i = 0; i < strlen(Text); i++)
		if (Text[i] == '\n')
			LineFeats++;

	if (!GUIUseCoustumPos)
	{
		TextPos = Vec3
		(
			GUIComponents[GUIWindowCount].Pos.x + GUIWindowBoarder,
			GUIComponentCount == 1 ? GUIComponents[GUIComponentCount - 1].Pos.y + GUIWindowBoarder
			:
			GUIComponents[GUIComponentCount - 1].Pos.y + GUIComponents[GUIComponentCount - 1].Size.y + 10,
			0.5
		);
	}
	else
		TextPos = GUICoustumPos;

	TextPos.y += (float)LineFeats * GUIFontSize;
	GUIComponents[GUIComponentCount].Pos = Vec2(TextPos.x, TextPos.y);
	GUIComponents[GUIComponentCount].Size = Vec2(GUIFontSize, GUIFontSize);
	GUIComponents[GUIComponentCount].Color = GUIFragmentConst.Color;
	GUIComponents[GUIComponentCount].Name = Text;
	GUIComponents[GUIComponentCount].ID = GUIWindowCount;

	GUIDrawText(GUIComponents[GUIComponentCount].Pos, GUIComponents[GUIComponentCount].Size, Text);

	GUIComponentCount++;
	GUIUseCoustumPos = false;
}

void GUIBeginDraw()
{
	GUIComponentCount = 0;
	GUIWindowCount = 0;

	vkCmdBindPipeline(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, GUIGraphicsPipeline);

	vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, GUIPipelineLayout, 1, 1, &OffScreenPass[0].DescriptorSet, 0, NULL);
	vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, GUIPipelineLayout, 2, 1, &OffScreenPass[1].DescriptorSet, 0, NULL);
	vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, GUIPipelineLayout, 3, 1, &OffScreenPass[3].DescriptorSet, 0, NULL);

	VkDeviceSize Offsets[] = { 0 };
	vkCmdBindVertexBuffers(CommandBuffers[ImageIndex], 0, 1, &GUIVertexBuffers[0], Offsets);
	vkCmdBindIndexBuffer(CommandBuffers[ImageIndex], GUIIndexBuffers[0], 0, VK_INDEX_TYPE_UINT32);

	VertexConst.Proj = OrthoMatrix(0.0, SwapChainExtent.width, SwapChainExtent.height, 0.0, -1.0, 1.0);
	vec3 ViewPos = { 0.0, 0.0, 1.0 };
	vec3 ViewLookAt = { 0.0, 0.0, 0.0 };

	VertexConst.View = LookAt
	(
		ViewPos,
		ViewLookAt,
		CameraUp
	);

	MouseState = SDL_GetMouseState(&MousePosX, &MousePosY);
}

void GUISetPos(vec2 Pos)
{
	GUIUseCoustumPos = true;
	GUICoustumPos = Vec3(Pos.x, Pos.y, 0.5);
}

void GUIEndDraw()
{
	GUIFirstInit = false;
	GUIMouseWheel = 0;
}

void GUIPollEvent()
{
	if (Event.type == SDL_MOUSEBUTTONDOWN && Event.button.button == SDL_BUTTON_LEFT)
		MouseDown = true;
	
	if (Event.type == SDL_MOUSEBUTTONUP && Event.button.button == SDL_BUTTON_LEFT)
		MouseDown = false;

	if (Event.wheel.y > 0)
		GUIMouseWheel = 1;

	if (Event.wheel.y < 0)
		GUIMouseWheel = 2;
}