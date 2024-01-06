#define CUBE_COUNT 8
//8
vec3 CubePositions[CUBE_COUNT] =
{
	0.0, 0.0, -1.5,
	1.0, 0.0, -5.5,
	3.5, 0.0, 2.5,
	7.0, 0.0, -4.0,
	-3.5, 0.0, -6.0,
	-6.5, 0.0, -4.5,
	-6.5, 0.0, 6.0,
};
vec3 CubeScales[CUBE_COUNT] =
{
	0.500000, 0.500000, 0.600000,
	0.750000, 0.500000, 1.000000,
	0.500000, 0.500000, 0.650000,
	0.900000, 0.500000, 0.800000,
	0.500000, 0.500000, 0.500000,
	0.950000, 0.500000, 0.700000,
	0.500000, 0.500000, 0.500000,
	0.000000, 0.000000, 0.000000,
};

vec3 PlanePos = { 0.0, -0.5, 0.0 };
vec3 PlaneScale = { 16.0, 0.1, 16.0 };

float CamerFallSpeed = 0.0;
float Gravity = 0.0;
float CollisionY = -0.5;

void PointVsBox(vec3* a, vec3* b, vec3 Scale)
{
	Scale.x += 0.15;
	Scale.y += 0.5;
	Scale.z += 0.15;

	//Collision for X
	if (a->z <= b->z + Scale.z && a->z >= b->z - Scale.z)
		if (a->y <= b->y + Scale.y && a->y >= b->y - Scale.y)
			if (a->x <= b->x + Scale.x && a->x >= b->x + (Scale.x - 0.2))
				a->x = b->x + Scale.x;

	if (a->z <= b->z + Scale.z && a->z >= b->z - Scale.z)
		if (a->y <= b->y + Scale.y && a->y >= b->y - Scale.y)
			if (a->x >= b->x - Scale.x && a->x <= b->x - (Scale.x - 0.2))
				a->x = b->x - Scale.x;

	//Collision for Y
	if (a->z <= b->z + Scale.z && a->z >= b->z - Scale.z)
		if (a->x <= b->x + Scale.x && a->x >= b->x - Scale.x)
			if (a->y <= b->y + Scale.y && a->y >= b->y + (Scale.y - 0.3))
			{
				CollisionY = b->y + Scale.y;
				a->y = b->y + Scale.y;
			}

	if (a->z <= b->z + Scale.z && a->z >= b->z - Scale.z)
		if (a->x <= b->x + Scale.x && a->x >= b->x - Scale.x)
			if (a->y >= b->y - Scale.y && a->y <= b->y - (Scale.y - 0.2))
				a->y = b->y - Scale.y;

	//Collision for Z
	if (a->x <= b->x + Scale.x && a->x >= b->x - Scale.x)
		if (a->y <= b->y + Scale.y && a->y >= b->y - Scale.y)
			if (a->z <= b->z + Scale.z && a->z >= b->z + (Scale.z - 0.2))
				a->z = b->z + Scale.z;

	if (a->x <= b->x + Scale.x && a->x >= b->x - Scale.x)
		if (a->y <= b->y + Scale.y && a->y >= b->y - Scale.y)
			if (a->z >= b->z - Scale.z && a->z <= b->z - (Scale.z - 0.2))
				a->z = b->z - Scale.z;
}

void SandboxInit()
{
	unsigned int Time = SDL_GetTicks();

	WaveModelData ModelData = WaveLoadModel("SampleHouse.obj", WAVE_FLIP_UVS | WAVE_GEN_NORMALS | WAVE_LOAD_MATERIAL);
	printf("Loading Time: %d ms\n", SDL_GetTicks() - Time);
	printf("Triangles: %d\n\n", ModelData.Mesh->VerticeCount / 3);
	ModelToVertex(&ModelData, &ModelVertices[0]);
	Time = SDL_GetTicks();

	ModelData = WaveLoadModel("viking_room.dae", WAVE_FLIP_UVS | WAVE_GEN_NORMALS | WAVE_LOAD_MATERIAL);
	printf("Loading Time: %d ms\n", SDL_GetTicks() - Time);
	printf("Triangles: %d\n\n", ModelData.Mesh->VerticeCount / 3);
	ModelToVertex(&ModelData, &ModelVertices[1]);
	Time = SDL_GetTicks();

	ModelData = WaveLoadModel("Vulkan.obj", WAVE_FLIP_UVS | WAVE_GEN_NORMALS | WAVE_LOAD_MATERIAL);
	printf("Loading Time: %d ms\n", SDL_GetTicks() - Time);
	printf("Triangles: %d\n\n", ModelData.Mesh->VerticeCount / 3);
	ModelToVertex(&ModelData, &ModelVertices[2]);
	Time = SDL_GetTicks();

	ModelData = WaveLoadModel("Cube.obj", WAVE_FLIP_UVS | WAVE_GEN_UVS | WAVE_GEN_NORMALS | WAVE_LOAD_MATERIAL);
	printf("Loading Time: %d ms\n", SDL_GetTicks() - Time);
	printf("Triangles: %d\n\n", ModelData.Mesh->VerticeCount / 3);
	ModelToVertex(&ModelData, &ModelVertices[3]);

	CreateSphere(25, 25);

	if (CreateVertexBuffer(&ModelVertexBuffer[0], &ModelVertices[0]) != VK_SUCCESS)
		printf("Failed to Create Vertex Buffer\n");

	if (CreateVertexBuffer(&ModelVertexBuffer[1], &ModelVertices[1]) != VK_SUCCESS)
		printf("Failed to Create Vertex Buffer\n");

	if (CreateVertexBuffer(&ModelVertexBuffer[2], &ModelVertices[2]) != VK_SUCCESS)
		printf("Failed to Create Vertex Buffer\n");

	if (CreateVertexBuffer(&ModelVertexBuffer[3], &ModelVertices[3]) != VK_SUCCESS)
		printf("Failed to Create Vertex Buffer\n");

	if (CreateVertexBuffer(&SphereVertexBuffer, &SphereVertices) != VK_SUCCESS)
		printf("Failed to Create Vertex Buffer\n");

	if (CreateIndexBuffer(&SphereIndexBuffer, &SphereIndices) != VK_SUCCESS)
		printf("Failed to Create Index Buffer\n");


	if (CreateTextureImage("viking_room.png") != VK_SUCCESS)
		printf("Faild To Create Texture Image\n");

	if (CreateTextureDescriptorSet() != VK_SUCCESS)
		printf("Failed to Create Texture Descriptor Set\n");
}

void SandboxUpdate()
{
	float DT = DeltaTime() * 0.006;

	const Uint8* State = SDL_GetKeyboardState(NULL);

	if (State[SDL_SCANCODE_LSHIFT])
		DT *= 2.0;
	else
		DT *= 1.0;

	vec3 CamerSpeed = { DT, DT, DT };

	vec3 TempCamera =
	{
		CameraFront.x,
		0.0,
		CameraFront.z
	};

	if (State[SDL_SCANCODE_W])
		CameraPos = Add3(CameraPos, Mul3(CamerSpeed, TempCamera));

	if (State[SDL_SCANCODE_S])
		CameraPos = Sub3(CameraPos, Mul3(CamerSpeed, TempCamera));

	if (State[SDL_SCANCODE_A])
		CameraPos = Sub3(CameraPos, Mul3(Normalize3(Cross(TempCamera, CameraUp)), CamerSpeed));

	if (State[SDL_SCANCODE_D])
		CameraPos = Add3(CameraPos, Mul3(Normalize3(Cross(TempCamera, CameraUp)), CamerSpeed));

	if (State[SDL_SCANCODE_SPACE] && CameraPos.y <= CollisionY)
		CamerFallSpeed += 0.25;


	static float Yaw = 90.0;
	static float Pitch = 0.0;

	int MouseX;
	int MouseY;

	SDL_GetMouseState(&MouseX, &MouseY);
	SDL_WarpMouseInWindow(Window, Width * 0.5, Height * 0.5);

	float Xoffset = (MouseX - Width * 0.5);
	float Yoffset = (MouseY - Height * 0.5);

	float Sensitivity = 0.06;
	Xoffset *= Sensitivity;
	Yoffset *= Sensitivity;

	Yaw += Xoffset;
	Pitch += Yoffset;

	if (Pitch > 89.0f)
		Pitch = 89.0f;
	if (Pitch < -89.0f)
		Pitch = -89.0f;

	vec3 Direction =
	{
		cosf(Radians(Yaw)) * cosf(Radians(Pitch)),
		-sinf(Radians(Pitch)),
		sinf(Radians(Yaw)) * cosf(Radians(Pitch))
	};
	CameraFront = Normalize3(Direction);

	CollisionY = -0.5;

	for (int i = 0; i < CUBE_COUNT; i++)
		PointVsBox(&CameraPos, &CubePositions[i], CubeScales[i]);

	PointVsBox(&CameraPos, &PlanePos, PlaneScale);

	Gravity = -0.01;

	CamerFallSpeed += Gravity;
	CameraPos.y += CamerFallSpeed;

	if (CameraPos.y <= CollisionY)
	{
		CameraPos.y = CollisionY;
		CamerFallSpeed = 0.0;
	}


	UBO.View = LookAt(CameraPos,
		Add3(CameraPos, CameraFront),
		CameraUp);
	float Aspect = (float)SwapChainExtent.width / SwapChainExtent.height;
	UBO.Proj = PerspectivMatrix(Aspect, 75.0, 0.01, 10000.0);
	//	UBO.Proj.m[1][1] *= -1;

	UBO.LightPos.x = 8.0 * sinf(SDL_GetTicks() * 0.001);
	UBO.LightPos.y = 4.0;
	UBO.LightPos.z = 8.0 * cosf(SDL_GetTicks() * 0.001);
}

void SandboxRender()
{
	FragmentConst.UseTextures = false;
	vkCmdPushConstants(CommandBuffers[ImageIndex], PipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, FRAGMENT_OFFSET, sizeof(FragmentPushConstant), &FragmentConst);

	VertexConst.Model = LoadMat4Identity();
	VertexConst.Model = ScaleMat4(VertexConst.Model, 0.5, 0.5, 0.5);
	VertexConst.Model = RotateYMat4(VertexConst.Model, 0.0);
	VertexConst.Model = TranslateMat4(VertexConst.Model, 1.0, 0.0, -1.0);

	BindVertices(&ModelVertexBuffer[0], 0);
	DrawVertices(&ModelVertices[0]);

	VertexConst.Model = LoadMat4Identity();
	VertexConst.Model = ScaleMat4(VertexConst.Model, 0.5, 0.5, 0.5);
	VertexConst.Model = RotateYMat4(VertexConst.Model, 0.0);
	VertexConst.Model = TranslateMat4(VertexConst.Model, 8.0 * sinf(SDL_GetTicks() * 0.001), 1.0, 8.0 * cosf(SDL_GetTicks() * 0.001));

	BindVertices(&ModelVertexBuffer[0], 0, &PipelineLayout);
	DrawVertices(&ModelVertices[0], &PipelineLayout);

	FragmentConst.UseTextures = true;
	vkCmdPushConstants(CommandBuffers[ImageIndex], PipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, FRAGMENT_OFFSET, sizeof(FragmentPushConstant), &FragmentConst);

	VertexConst.Model = LoadMat4Identity();
	VertexConst.Model = ScaleMat4(VertexConst.Model, 0.5, 0.5, 0.5);
	//	VertexConst.Model = RotateYMat4(VertexConst.Model, SDL_GetTicks() * 0.001);
	VertexConst.Model = TranslateMat4(VertexConst.Model, -3.0, -0.3, -1.0);

	BindVertices(&ModelVertexBuffer[1], 3, &PipelineLayout);
	DrawVertices(&ModelVertices[1], &PipelineLayout);

	FragmentConst.UseTextures = false;
	vkCmdPushConstants(CommandBuffers[ImageIndex], PipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, FRAGMENT_OFFSET, sizeof(FragmentPushConstant), &FragmentConst);

	VertexConst.Model = LoadMat4Identity();
	VertexConst.Model = ScaleMat4(VertexConst.Model, 0.5, 0.5, 0.5);

	VertexConst.Model = RotateXMat4(VertexConst.Model, SDL_GetTicks() * 0.001);
	VertexConst.Model = RotateYMat4(VertexConst.Model, SDL_GetTicks() * 0.003);
	VertexConst.Model = RotateZMat4(VertexConst.Model, SDL_GetTicks() * 0.002);

	VertexConst.Model = TranslateMat4(VertexConst.Model, 0.0, 0.0, -1.0);

	FragmentConst.UseTextures = true;
	vkCmdPushConstants(CommandBuffers[ImageIndex], PipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, FRAGMENT_OFFSET, sizeof(FragmentPushConstant), &FragmentConst);

	BindIndices(&SphereVertexBuffer, &SphereIndexBuffer, 1, &PipelineLayout);
	DrawIndices(&SphereIndices, &PipelineLayout);

	FragmentConst.UseTextures = false;
	vkCmdPushConstants(CommandBuffers[ImageIndex], PipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, FRAGMENT_OFFSET, sizeof(FragmentPushConstant), &FragmentConst);


	VertexConst.Model = LoadMat4Identity();
	VertexConst.Model = ScaleMat4(VertexConst.Model, 2.0, 2.0, 2.0);
	VertexConst.Model = RotateYMat4(VertexConst.Model, SDL_GetTicks() * 0.001);
	//	VertexConst.Model = RotateXMat4(VertexConst.Model, Radians(-90.0));

	BindVertices(&ModelVertexBuffer[2], 0, &PipelineLayout);

	for (float z = 0.0; z < 800; z += 2)
	{
		vec3 ModelPos =
		{
			6.0 * sin(z / 2),
			0.0,
			-z,
		};

		VertexConst.Model = TranslateMat4(VertexConst.Model, ModelPos.x, ModelPos.y, ModelPos.z);

		float Distance = GetDistanceBetween2Points(ModelPos, CameraPos);

		if (Distance <= 30)
		{
			
			DrawVertices(&ModelVertices[2], &PipelineLayout);
		}

	}

	FragmentConst.UseTextures = true;
	vkCmdPushConstants(CommandBuffers[ImageIndex], PipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, FRAGMENT_OFFSET, sizeof(FragmentPushConstant), &FragmentConst);

	BindVertices(&ModelVertexBuffer[3], 2, &PipelineLayout);

	for (int i = 0; i < CUBE_COUNT; i++)
	{
		VertexConst.Model = LoadMat4Identity();
		VertexConst.Model = ScaleMat4V(VertexConst.Model, CubeScales[i]);
		VertexConst.Model = TranslateMat4V(VertexConst.Model, CubePositions[i]);

		
		DrawVertices(&ModelVertices[3], &PipelineLayout);
	}

	VertexConst.Model = LoadMat4Identity();
	VertexConst.Model = ScaleMat4V(VertexConst.Model, PlaneScale);
	VertexConst.Model = TranslateMat4V(VertexConst.Model, PlanePos);

	BindVertices(&ModelVertexBuffer[3], 2, &PipelineLayout);
	DrawVertices(&ModelVertices[3], &PipelineLayout);
}

void SandboxDeInit()
{

}
