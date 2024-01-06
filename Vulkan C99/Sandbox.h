bool MoveLight = true;

unsigned int WoodTexture;
unsigned int OrangeTexture;

float LightSpeed = 0.0005;

void LoadModelFromFile()
{
	char Path[4096];
	if (OpenDialog(Path, OBJDAESTLWAVE))
	{
		printf("%s\n", Path);
	
		WaveModelData ModelData = WaveLoadModel(Path, WAVE_GEN_UVS | WAVE_FLIP_UVS | WAVE_GEN_NORMALS | WAVE_LOAD_MATERIAL);
		printf("Triangles: %d\n\n", ModelData.Mesh->VerticeCount / 3);
		ModelToVertex(&ModelData, &ModelVertices);
		vkDeviceWaitIdle(Device);
		vkDestroyBuffer(Device, ModelVertexBuffer, NULL);

		if (CreateVertexBuffer(&ModelVertexBuffer, &ModelVertices) != VK_SUCCESS)
			printf("Failed to Create Vertex Buffer\n");
	}
}

void SandboxInit()
{
	unsigned int Time = SDL_GetTicks();

	WaveModelData ModelData = WaveLoadModel("Bunny.obj", WAVE_GEN_UVS | WAVE_GEN_NORMALS | WAVE_LOAD_MATERIAL);
	
//	WaveModelData ModelData = WaveLoadWave("C:/Models/Bugatti.wave", WAVE_GEN_UVS);

	printf("Loading Time: %d ms\n", SDL_GetTicks() - Time);
	printf("Triangles: %d\n\n", ModelData.Mesh->VerticeCount / 3);
	
	ModelToVertex(&ModelData, &ModelVertices);
	Time = SDL_GetTicks();
	
//	exit(0);

	GUIInit();

	QuadVertices3D.Size = 6;
	QuadVertices3D.Vertex = malloc(4 * sizeof(VertexCore));

	QuadVertices3D.Vertex[0].Pos = Vec3(-1.0, 1.0, 0.0);
	QuadVertices3D.Vertex[1].Pos = Vec3(1.0, 1.0, 0.0);
	QuadVertices3D.Vertex[2].Pos = Vec3(1.0, -1.0, 0.0);
	QuadVertices3D.Vertex[3].Pos = Vec3(-1.0, -1.0, 0.0);
	
	QuadVertices3D.Vertex[0].Color = Vec3(1.0, 1.0, 1.0);
	QuadVertices3D.Vertex[1].Color = Vec3(1.0, 1.0, 1.0);
	QuadVertices3D.Vertex[2].Color = Vec3(1.0, 1.0, 1.0);
	QuadVertices3D.Vertex[3].Color = Vec3(1.0, 1.0, 1.0);
	
	QuadVertices3D.Vertex[0].TexCoord = Vec2(0.0, 1.0);
	QuadVertices3D.Vertex[1].TexCoord = Vec2(1.0, 1.0);
	QuadVertices3D.Vertex[2].TexCoord = Vec2(1.0, 0.0);
	QuadVertices3D.Vertex[3].TexCoord = Vec2(0.0, 0.0);

	QuadVertices3D.Vertex[0].Normal = Vec3(0.0, 0.0, -1.0);
	QuadVertices3D.Vertex[1].Normal = Vec3(0.0, 0.0, -1.0);
	QuadVertices3D.Vertex[2].Normal = Vec3(0.0, 0.0, -1.0);
	QuadVertices3D.Vertex[3].Normal = Vec3(0.0, 0.0, -1.0);

	QuadIndices3D.Size = 6;
	QuadIndices3D.Index = malloc(6 * sizeof(int));
	QuadIndices3D.Index[0] = 0;
	QuadIndices3D.Index[1] = 1;
	QuadIndices3D.Index[2] = 2;
			   
	QuadIndices3D.Index[3] = 2;
	QuadIndices3D.Index[4] = 3;
	QuadIndices3D.Index[5] = 0;


	if (CreateVertexBuffer(&QuadVertexBuffer3D, &QuadVertices3D) != VK_SUCCESS)
		printf("Failed to Create Vertex Buffer\n");

	if (CreateIndexBuffer(&QuadIndexBuffer3D, &QuadIndices3D) != VK_SUCCESS)
		printf("Failed to Create Index Buffer\n");

	CreateSphere(15, 15);

	if (CreateVertexBuffer(&ModelVertexBuffer, &ModelVertices) != VK_SUCCESS)
		printf("Failed to Create Vertex Buffer\n");

	if (CreateVertexBuffer(&SphereVertexBuffer, &SphereVertices) != VK_SUCCESS)
		printf("Failed to Create Vertex Buffer\n");

	if (CreateIndexBuffer(&SphereIndexBuffer, &SphereIndices) != VK_SUCCESS)
		printf("Failed to Create Index Buffer\n");

	WoodTexture = CreateTextureImage("WoodFloor.jpg", true);
	OrangeTexture = CreateTextureImage("Earth.png", true);

	GUITextures[0] = CreateTextureImage("Icons/CheckBox0.png", false);
	GUITextures[1] = CreateTextureImage("Icons/CheckBox1.png", false);
	GUITextures[2] = CreateTextureImage("Icons/Circle.png", false);
	GUITextures[3] = CreateTextureImage("Icons/Button.png", false);
	GUITextures[4] = CreateTextureImage("Fonts/SDF Consolas.png", false);

	CreateTextureDescriptorSets();
}

void SandboxUpdate()
{
	float DT = DeltaTime() * 0.006;

	const Uint8* State = SDL_GetKeyboardState(NULL);

	if (State[SDL_SCANCODE_LSHIFT])
		DT *= 1.0;
	else
		DT *= 0.5;

	vec3 CamerSpeed = { DT, DT, DT };

	if (State[SDL_SCANCODE_W])
		CameraPos = Add3(CameraPos, Mul3(CamerSpeed, CameraFront));

	if (State[SDL_SCANCODE_S])
		CameraPos = Sub3(CameraPos, Mul3(CamerSpeed, CameraFront));

	if (State[SDL_SCANCODE_A])
		CameraPos = Sub3(CameraPos, Mul3(Normalize3(Cross(CameraFront, CameraUp)), CamerSpeed));

	if (State[SDL_SCANCODE_D])
		CameraPos = Add3(CameraPos, Mul3(Normalize3(Cross(CameraFront, CameraUp)), CamerSpeed));

	static float Yaw = 90.0;
	static float Pitch = 45.0;

	static float DeltaX = 0.0;
	static float DeltaY = 0.0;
	 
	static float MouseDeltaX = 0.0;
	static float MouseDeltaY = 0.0;

	static bool FirstInit = true;

	int MouseX;
	int MouseY;

	unsigned int MouseState = SDL_GetMouseState(&MouseX, &MouseY);

	if (FirstInit)
	{
		FirstInit = false;
		goto InitMouse;
	}

	if (MouseState == 4)
	{
	InitMouse:
		if (MouseDeltaX != 0.0)
		{
			DeltaX = (MouseDeltaX - MouseX);
			DeltaY = (MouseDeltaY - MouseY);
		}
		else
			DeltaX = DeltaY = 0.0;

		MouseDeltaX = MouseX;
		MouseDeltaY = MouseY;

		float sensitivity = 0.1f;

		Yaw -= (DeltaX * sensitivity);
		Pitch -= (DeltaY * sensitivity);

		if (Pitch >= 89.0)
			Pitch = 89.0;
		if (Pitch <= -89.0)
			Pitch = -89.0;

		vec3 Direction =
		{
			cosf(Radians(Yaw)) * cosf(Radians(Pitch)),
			-sinf(Radians(Pitch)),
			sinf(Radians(Yaw)) * cosf(Radians(Pitch))
		};
		CameraFront = Normalize3(Direction);
	}
	else
	{
		MouseDeltaX = 0.0;
		MouseDeltaY = 0.0;
		DeltaX = 0.0;
		DeltaY = 0.0;
	}

	if (MoveLight)
	{
		UBO.LightPos.x = 8 * sinf(LightSpeed);
		UBO.LightPos.y = 8;
		UBO.LightPos.z = 8 * cosf(LightSpeed);
	}
	else
	{
		UBO.LightPos.x = 8 * sinf(SDL_GetTicks() * 0.0005);
		UBO.LightPos.y = 8;
		UBO.LightPos.z = 8 * cosf(SDL_GetTicks() * 0.0005);
	}

	UBO.ClearColor = ClearColor;
}

vec3 ModelPos = { 0.0, 0.0, 0.0 };
vec3 ModelScale = { 2.5, 2.5, 2.5 };

bool ModelHasTexture = false;

void RenderScene(bool HasFragment, bool BindDescriptorSets, VkPipeline Pipeline, VkPipelineLayout PipelineLayout)
{
	vkCmdBindPipeline(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline);

	if (HasFragment)
	{
		FragmentConst.UseTextures = ModelHasTexture;
		vkCmdPushConstants(CommandBuffers[ImageIndex], PipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, FRAGMENT_OFFSET, sizeof(FragmentPushConstant), &FragmentConst);
	}

	VertexConst.Model = LoadMat4Identity();
	VertexConst.Model = ScaleMat4V(VertexConst.Model, ModelScale);
	VertexConst.Model = RotateYMat4(VertexConst.Model, Radians(180.0));
	VertexConst.Model = TranslateMat4V(VertexConst.Model, ModelPos);

	BindVertices(&ModelVertexBuffer, OrangeTexture, &PipelineLayout, BindDescriptorSets);
	DrawVertices(&ModelVertices, &PipelineLayout);
	

	if (HasFragment)
	{
		FragmentConst.UseTextures = true;
		vkCmdPushConstants(CommandBuffers[ImageIndex], PipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, FRAGMENT_OFFSET, sizeof(FragmentPushConstant), &FragmentConst);
	}
	VertexConst.Model = LoadMat4Identity();
	VertexConst.Model = ScaleMat4(VertexConst.Model, 6.0, 6.0, 1.0);
	VertexConst.Model = RotateXMat4(VertexConst.Model, Radians(90.0));
	VertexConst.Model = TranslateMat4(VertexConst.Model, 0.0, 0.0, 0.0);

	BindIndices(&QuadVertexBuffer3D, &QuadIndexBuffer3D, 0, &PipelineLayout, BindDescriptorSets);
	DrawIndices(&QuadIndices3D, &PipelineLayout);
}

void SandboxRenderDepth()
{
//	VertexConst.View = LookAt
//	(
//		CameraPos,
//		Add3(CameraPos, CameraFront),
//		CameraUp
//	);
//
//	float Aspect = (float)OffScreenPass[1].Width / OffScreenPass[1].Height;
//	VertexConst.Proj = PerspectivMatrix(Aspect, 75.0, 0.01, 10000.0);
//
//
//	float Near = 0.01;
//	float Far = 10000.0;
//	float ClipRange = Far - Near;
//
//	float MinZ = Near;
//	float MaxZ = Near + ClipRange;
//
//	float Range = MaxZ - MinZ;
//	float Ratio = MaxZ / MinZ;
//
//	float p = 1;
//	float log = MinZ * powf(Ratio, p);
//	float uniform = MinZ + Range * p;
//	float d = 0.95 * (log - uniform) + uniform;
//	float Split = (d - Near) / ClipRange;
//
//	vec3 frustumCorners[8] = 
//	{
//		-1.0f,  1.0f, -1.0f,
//		1.0f,  1.0f, -1.0f,
//		1.0f, -1.0f, -1.0f,
//		-1.0f, -1.0f, -1.0f,
//		-1.0f,  1.0f,  1.0f,
//		1.0f,  1.0f,  1.0f,
//		1.0f, -1.0f,  1.0f,
//		-1.0f, -1.0f,  1.0f,
//	};
//	
//	mat4 ProjView = MultiplyMat4(VertexConst.Proj, VertexConst.View);
//	mat4 IverseCam = LoadMat4Identity();
//
//	InverseMat4(&ProjView, &IverseCam);
//
//	for (uint32_t i = 0; i < 8; i++) 
//	{
//		vec4 invCorner = MultiplyVec4Mat4(Vec4(frustumCorners[i].x, frustumCorners[i].y, frustumCorners[i].z, 1.0f), IverseCam);
//		frustumCorners[i].x = invCorner.x / invCorner.w;
//		frustumCorners[i].y = invCorner.y / invCorner.w;
//		frustumCorners[i].z = invCorner.z / invCorner.w;
//	}
//
//	for (uint32_t i = 0; i < 4; i++) 
//	{
//		vec3 dist = Sub3(frustumCorners[i + 4], frustumCorners[i]);
//		frustumCorners[i + 4] = Add3(frustumCorners[i], Mul3(dist, Vec3(Split, Split, Split)));
//		frustumCorners[i] = Add3(frustumCorners[i], Vec3(0, 0, 0));
//	}
//
//	vec3 frustumCenter = Vec3(0, 0, 0);
//	for (uint32_t i = 0; i < 8; i++) {
//		frustumCenter = Add3(frustumCorners[i], frustumCenter);
//	}
//
//	frustumCenter = Div3(frustumCenter, Vec3(8, 8, 8));
//
//	float radius = 0.0f;
//	for (uint32_t i = 0; i < 8; i++) 
//	{
//		float distance = Lenght3(Sub3(frustumCorners[i], frustumCenter));
//		radius = MAX(radius, distance);
//	}
//	radius = ceilf(radius * 16.0f) / 16.0f;
//
//	vec3 maxExtents = Vec3(radius, radius, radius);
//	vec3 minExtents = Vec3(-maxExtents.x, -maxExtents.y, -maxExtents.z);
//
//	vec3 lightDir = Normalize3(Vec3(-UBO.LightPos.x, -UBO.LightPos.y, -UBO.LightPos.z));
//	mat4 lightViewMatrix = LookAt(Sub3(frustumCenter, Mul3(lightDir, Vec3(-minExtents.z, -minExtents.z, -minExtents.z))), frustumCenter, Vec3(0.0f, 1.0f, 0.0f));
//	mat4 lightOrthoMatrix = OrthoMatrix(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, maxExtents.z - minExtents.z);
//
//	VertexConst.View = lightViewMatrix;
//	VertexConst.Proj = lightOrthoMatrix;

	vec3 LightPos = { UBO.LightPos.x, UBO.LightPos.y, UBO.LightPos.z };
	vec3 LightLookAt = { 0.0, 0.0, 0.0 };

	VertexConst.View = LookAt
	(
		LightPos,
		Vec3(0.0, 0.0, 0.0),
		CameraUp
	);

	float Aspect = (float)OffScreenPass[0].Width / OffScreenPass[0].Height;
	VertexConst.Proj = PerspectivMatrix(Aspect, 60, 1.0, 1000.0);
//	VertexConst.Proj = OrthoMatrix(-10.0, 10.0, -10.0, 10.0, -22.0, 22.0);
	UBO.LightMat = MultiplyMat4(VertexConst.Proj, VertexConst.View);

	RenderScene(false, false, OffScreenPass[0].GraphicsPipeline, OffScreenPass[0].PipelineLayout);
}

void SandboxRender()
{
	VertexConst.View = LookAt
	(
		CameraPos,
		Add3(CameraPos, CameraFront),
		CameraUp
	);

	float Aspect = (float)OffScreenPass[1].Width / OffScreenPass[1].Height;
	VertexConst.Proj = PerspectivMatrix(Aspect, 75.0, 0.01, 10000.0);

	RenderScene(true, true, OffScreenPass[1].GraphicsPipeline, OffScreenPass[1].PipelineLayout);

	VertexConst.Model = LoadMat4Identity();
	VertexConst.Model = ScaleMat4(VertexConst.Model, 0.5, 0.5, 0.5);
	VertexConst.Model = RotateYMat4(VertexConst.Model, 0.0);
	VertexConst.Model = TranslateMat4(VertexConst.Model, UBO.LightPos.x, UBO.LightPos.y, UBO.LightPos.z);

	BindIndices(&SphereVertexBuffer, &SphereIndexBuffer, 0, &OffScreenPass[1].PipelineLayout, true);
	DrawIndices(&SphereIndices, &OffScreenPass[1].PipelineLayout);
}

void SandboxRenderGUI()
{
	GUIBeginDraw();
	{
		GUIBeginWindow("Dummy", Vec2(Width - 400, 0.0), Vec2(400, OffScreenPass[1].Height));
		{
			static float Scale = 2.5;
			static float FilterLevel = 0.0;

			GUISlider("Brightness", &FragmentConst.LightBrightness, 0.1, 6.0);
			GUISlider("Rotation", &LightSpeed, 0.0, 5.0);

			GUISlider("Pos z", &ModelPos.z, -20.0, 20.0);
			GUISlider("Pos y", &ModelPos.y, -10.0, 10.0);
			GUISlider("Pos x", &ModelPos.x, -20.0, 20.0);
			GUISlider("Scale", &Scale, -20.0, 20.0);
			GUISlider("Filter", &FilterLevel, 0.0, 6.0);
			FragmentConst.FilterLevel = FilterLevel;
			ModelScale = Vec3(Scale, Scale, Scale);

			if (GUIButton("Load Model"))
				LoadModel = true;

			GUICheckBox("Rotate", &MoveLight);
			GUICheckBox("Texture", &ModelHasTexture);

			if (GUIButton("Load Texture"))
				LoadTexture = true;

			GUIText("Hello there\nIts me\nIm a Dummy Text");

			GUIImageRenderMode = 5;
			GUIImage("Shadow", 0, Vec2(200, 200));

			GUIImageRenderMode = 4;
			GUIImage("Blur", 0, Vec2(200, 200));

			GUIImageRenderMode = 6;
			GUIImage("Default", 0, Vec2(200, 200));
		}
		GUIEndWindow();

		GUITexturedWindow(Vec2(0, 0), Vec2(OffScreenPass[1].Width, OffScreenPass[1].Height), 3);
		GUIDrawText(Vec2(0, OffScreenPass[1].Height - 30), Vec2(25, 25), CharFPS + 4);

		char RenderTimes[1024];
		sprintf(RenderTimes, "Shadow: %f ms\nScene: %f ms\nBlur: %f ms\nGUI: %f ms\n\nSubmit Queue: %f ms\nPresent Queue: %f ms", TimeToRenderDepth, TimeToRenderScene, TimeToRenderBlur, TimeToRenderGUI, TimeToSubmitQueue, TimeToPresentQueue);
		GUIDrawText(Vec2(0, OffScreenPass[1].Height - 80), Vec2(25, 25), RenderTimes);

	}	
	GUIEndDraw();
}

void SandboxDeInit()
{
	GUIDeInit();

//	FreeVertex(&ModelVertices, NULL);
//	FreeVertex(&SphereVertices, &SphereIndices);

	vkDestroyBuffer(Device, QuadVertexBuffer3D, NULL);
	vkDestroyBuffer(Device, QuadIndexBuffer3D, NULL);

	vkDestroyBuffer(Device, ModelVertexBuffer, NULL);

	vkDestroyBuffer(Device, SphereVertexBuffer, NULL);
	vkDestroyBuffer(Device, SphereIndexBuffer, NULL);
}