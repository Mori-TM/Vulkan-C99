#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define WAVEMAX(x, y) (((x) > (y)) ? (x) : (y))
#define WAVEMIN(x, y) (((x) < (y)) ? (x) : (y))
#define WAVE_ALLOCATION_CHUNK_SIZE 100000

typedef enum
{
	WAVE_GEN_UVS = 1,
	WAVE_GEN_NORMALS = 2,
	WAVE_FLIP_UVS = 4,
	WAVE_LOAD_MATERIAL = 8
} WaveSettings;

typedef struct
{
	float x;
	float y;
	float z;
} WaveVec3;

float WaveFastInvertSqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5f;

	x2 = number * 0.5f;
	y = number;
	i = *(long*)&y;              // evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);   // what the fuck? 
	y = *(float*)&i;
	y = y * (threehalfs - (x2 * y * y));   // 1st iteration
	y = y * (threehalfs - (x2 * y * y));   // 2nd iteration, this can be removed - it's for more accuracy

	return y;
}

WaveVec3 WaveNormalize(WaveVec3 v)
{
	float length = WaveFastInvertSqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	v.x *= length;
	v.y *= length;
	v.z *= length;

	return v;
}

WaveVec3 WaveCross(WaveVec3 a, WaveVec3 b)
{
	WaveVec3 r;
	r.x = a.y * b.z - a.z * b.y;
	r.y = a.z * b.x - a.x * b.z;
	r.z = a.x * b.y - a.y * b.x;

	return r;
}

float WaveDot(WaveVec3 a, WaveVec3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

WaveVec3 WaveAdd(WaveVec3 a, WaveVec3 b)
{
	WaveVec3 r;
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;

	return r;
}

WaveVec3 WaveSub(WaveVec3 a, WaveVec3 b)
{
	WaveVec3 r;
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	r.z = a.z - b.z;

	return r;
}

WaveVec3 WaveMul(WaveVec3 a, WaveVec3 b)
{
	WaveVec3 r;
	r.x = a.x * b.x;
	r.y = a.y * b.y;
	r.z = a.z * b.z;

	return r;
}

WaveVec3 WaveDiv(WaveVec3 a, WaveVec3 b)
{
	WaveVec3 r;
	r.x = a.x / b.x;
	r.y = a.y / b.y;
	r.z = a.z / b.z;

	return r;
}

typedef struct
{
	unsigned int MaterialCount;
	unsigned int VertexIndex;

	char MaterialName[2048];

	WaveVec3 AmbientColor;
	WaveVec3 DiffuseColor;
	unsigned int SpecularExponent;
	WaveVec3 SpecularColor;
	char AmbientTexture[4096];
	char DiffuseTexture[4096];
	char SpecularTexture[4096];
	char AlphaTexture[4096];
	char NormalTexture[4096];
	char HeightTexture[4096];
} WaveModelMaterial;

typedef struct
{
	unsigned int VerticeCount;
	WaveVec3 Vertices;
	WaveVec3 TexCoords;
	WaveVec3 Normals;
	WaveVec3 VertexColor;
} WaveMeshData;

typedef struct
{
	int HasMaterial;
	WaveModelMaterial* Material;

	WaveMeshData* Mesh;
} WaveModelData;

typedef struct
{
	char Name[2048];
	unsigned int Pos;
} WaveObjMaterialDescription;

void WaveGenUVs(WaveModelData* Data, unsigned int i)
{
	float MinX = 1000000000.0;
	float MaxX = -1000000000.0;

	float MinY = 1000000000.0;
	float MaxY = -1000000000.0;

	//		float TempUVsqrt = sqrtf(Data->Mesh[i].Vertices.x * Data->Mesh[i].Vertices.x + Data->Mesh[i].Vertices.y * Data->Mesh[i].Vertices.y + Data->Mesh[i].Vertices.z * Data->Mesh[i].Vertices.z);
	//		Data->Mesh[i].TexCoords.x = sinf(cosf(Data->Mesh[i].Vertices.x / TempUVsqrt));
	//		Data->Mesh[i].TexCoords.y = sinf(sinf(Data->Mesh[i].Vertices.y / TempUVsqrt));

	float TempUVsqrt = sqrtf(Data->Mesh[i].Vertices.x * Data->Mesh[i].Vertices.x + Data->Mesh[i].Vertices.y * Data->Mesh[i].Vertices.y + Data->Mesh[i].Vertices.z * Data->Mesh[i].Vertices.z);
	Data->Mesh[i].TexCoords.x = sinf(cosf(Data->Mesh[i].Vertices.x / TempUVsqrt));
	Data->Mesh[i].TexCoords.y = sinf(sinf(Data->Mesh[i].Vertices.y / TempUVsqrt));

	//	MinX = WAVEMIN(MinX, Data->Mesh[i].Vertices.x);
	//	MinY = WAVEMIN(MinY, Data->Mesh[i].Vertices.y);
	//
	//	MaxX = WAVEMAX(MaxX, Data->Mesh[i].Vertices.x);
	//	MaxY = WAVEMAX(MaxY, Data->Mesh[i].Vertices.y);
	//
	//	float KX = 1 / (MaxX - MinX);
	//	float KY = 1 / (MaxY - MinY);
	//
	//	Data->Mesh[i].TexCoords.x = (Data->Mesh[i].Vertices.x - MinX) * KX;
	//	Data->Mesh[i].TexCoords.y = (Data->Mesh[i].Vertices.y - MinY) * KY;

	/*
	float DotProduct = 2 * WaveDot(Data->Mesh[i].Normals, Data->Mesh[i].Vertices);
	float X = Data->Mesh[i].Normals.x * DotProduct - Data->Mesh[i].Vertices.x;
	float Y = Data->Mesh[i].Normals.y * DotProduct - Data->Mesh[i].Vertices.y;
	float Z = Data->Mesh[i].Normals.z * DotProduct - Data->Mesh[i].Vertices.z;
	WaveVec3 R = { X, Y, Z };

	if ((R.x >= R.y) && (R.x >= R.z))
	{
		float UC = -R.z;
		float VC = -R.y;
		float M = fabsf(R.x);

		Data->Mesh[i].TexCoords.x = ((UC / M) + 1) / 2;
		Data->Mesh[i].TexCoords.y = ((VC / M) + 1) / 2;
	}

	if ((R.x <= R.y) && (R.x <= R.z))
	{
		float UC = +R.z;
		float VC = -R.y;
		float M = fabsf(R.x);

		Data->Mesh[i].TexCoords.x = ((UC / M) + 1) / 2;
		Data->Mesh[i].TexCoords.y = ((VC / M) + 1) / 2;
	}

	if ((R.y >= R.z) && (R.y >= R.x))
	{
		float UC = +R.x;
		float VC = +R.z;
		float M = fabsf(R.y);

		Data->Mesh[i].TexCoords.x = ((UC / M) + 1) / 2;
		Data->Mesh[i].TexCoords.y = ((VC / M) + 1) / 2;
	}

	if ((R.y <= R.z) && (R.y <= R.x))
	{
		float UC = +R.x;
		float VC = -R.z;
		float M = fabsf(R.y);

		Data->Mesh[i].TexCoords.x = ((UC / M) + 1) / 2;
		Data->Mesh[i].TexCoords.y = ((VC / M) + 1) / 2;
	}

	if ((R.z >= R.y) && (R.z >= R.x))
	{
		float UC = +R.x;
		float VC = -R.y;
		float M = fabsf(R.z);

		Data->Mesh[i].TexCoords.x = ((UC / M) + 1) / 2;
		Data->Mesh[i].TexCoords.y = ((VC / M) + 1) / 2;
	}

	if ((R.z <= R.y) && (R.z <= R.x))
	{
		float UC = -R.x;
		float VC = -R.y;
		float M = fabsf(R.z);

		Data->Mesh[i].TexCoords.x = ((UC / M) + 1) / 2;
		Data->Mesh[i].TexCoords.y = ((VC / M) + 1) / 2;
	}
	*/
}

void WaveGenNormals(WaveModelData* Data)
{
	for (unsigned int i = 0; i < Data->Mesh->VerticeCount; i += 3)
	{
		WaveVec3 N = WaveCross(WaveSub(Data->Mesh[i + 1].Vertices, Data->Mesh[i].Vertices), WaveSub(Data->Mesh[i + 2].Vertices, Data->Mesh[i].Vertices));

		Data->Mesh[i].Normals = N;
		Data->Mesh[i + 1].Normals = N;
		Data->Mesh[i + 2].Normals = N;

		Data->Mesh[i].Normals = WaveNormalize(Data->Mesh[i].Normals);
		Data->Mesh[i + 1].Normals = WaveNormalize(Data->Mesh[i + 1].Normals);
		Data->Mesh[i + 2].Normals = WaveNormalize(Data->Mesh[i + 2].Normals);
	}
}

float WaveAngle(WaveVec3 a, WaveVec3 b)
{
	float Dot = WaveDot(a, b);
	float LenA = a.x * a.x + a.y * a.y + a.z * a.z;
	float LenB = b.x * b.x + b.y * b.y + b.z * b.z;

	float Angle = acos(Dot / sqrtf(LenA * LenB));

	return Angle;
}

void WaveGenSmoothNormals(WaveModelData* Data, float* Indices, unsigned int IndicesCount, float* Vertices, unsigned int VerticesCount)
{
	for (unsigned int i = 0; i < VerticesCount; i += 3)
	{
		if (Vertices[i] == Vertices[i + 1])
		{

		}

	}
}

typedef struct
{
	int V;
	int VT;
	int VN;
} VertexReference;

WaveModelData WaveLoadOBJ(const char* Path, WaveSettings Settings)
{
	FILE* File = fopen(Path, "r");
	if (!File)
		printf("Failed To Load Model!\n");

	unsigned int TempVerticeCount = 1;
	unsigned int TempUVCount = 1;
	unsigned int TempNormalCount = 1;

	unsigned int TempVerticeAllocationCount = WAVE_ALLOCATION_CHUNK_SIZE;
	unsigned int TempUVAllocationCount = 3;
	unsigned int TempNormalAllocationCount = 3;
	WaveVec3* TempVertices = (WaveVec3*)malloc(TempVerticeAllocationCount * sizeof(WaveVec3));
	WaveVec3* TempUVs = (WaveVec3*)malloc(TempUVAllocationCount * sizeof(WaveVec3));
	WaveVec3* TempNormals = (WaveVec3*)malloc(TempUVAllocationCount * sizeof(WaveVec3));

	int WaveHasMaterial = 0;
	int MaterialCount = -1;
	WaveModelMaterial* Material = (WaveModelMaterial*)malloc(1 * sizeof(WaveModelMaterial));

	int ObjMaterialCount = 0;
	WaveObjMaterialDescription* ObjMaterial = (WaveObjMaterialDescription*)malloc(1 * sizeof(WaveObjMaterialDescription));

	WaveModelMaterial EmptyMaterial =
	{
		1,
		0,
		"Default",
		{ 1.0, 1.0, 1.0 },
		{ 1.0, 1.0, 1.0 },
		1.0,
		{ 1.0, 1.0, 1.0 },
		"NoTexture",
		"NoTexture",
		"NoTexture",
		"NoTexture",
		"NoTexture",
		"NoTexture"
	};

	WaveModelData Data;
	unsigned int MeshCount = WAVE_ALLOCATION_CHUNK_SIZE;
	Data.Mesh = (WaveMeshData*)malloc(MeshCount * sizeof(WaveMeshData));
	Data.Mesh->VerticeCount = 0;

	VertexReference* VertexReferenc = (VertexReference*)malloc(4 * sizeof(VertexReference));
	unsigned int VertexReferencCount = 0;

	while (1)
	{
		char LineHeader[128];

		int Res = fscanf(File, "%s", LineHeader);
		if (Res == EOF)
			break;

		if (strcmp(LineHeader, "mtllib") == 0 && (Settings & WAVE_LOAD_MATERIAL))
		{
			char Path[2048];

			fscanf(File, "%[^\r\n]%*c\r\n", &Path);

			printf("%s\n", Path + 1);

			FILE* MatFile = fopen(Path + 1, "r");
			if (!MatFile)
				goto NoMatterial;
			else
				WaveHasMaterial = 1;

			while (1)
			{
				char MatLineHeader[128];

				int MatRes = fscanf(MatFile, "%s", MatLineHeader);
				if (MatRes == EOF)
					break;

				if (strcmp(MatLineHeader, "Ka") == 0)
					fscanf(MatFile, "%f %f %f\n", &Material[MaterialCount].AmbientColor.x, &Material[MaterialCount].AmbientColor.y, &Material[MaterialCount].AmbientColor.z);

				if (strcmp(MatLineHeader, "Kd") == 0)
					fscanf(MatFile, "%f %f %f\n", &Material[MaterialCount].DiffuseColor.x, &Material[MaterialCount].DiffuseColor.y, &Material[MaterialCount].DiffuseColor.z);

				if (strcmp(MatLineHeader, "Ks") == 0)
					fscanf(MatFile, "%f %f %f\n", &Material[MaterialCount].SpecularColor.x, &Material[MaterialCount].SpecularColor.y, &Material[MaterialCount].SpecularColor.z);

				if (strcmp(MatLineHeader, "Ns") == 0)
					fscanf(MatFile, "%f\n", &Material[MaterialCount].SpecularExponent);

				if (strcmp(MatLineHeader, "map_Ka") == 0)
					fscanf(MatFile, "%s\n", &Material[MaterialCount].AmbientTexture);

				if (strcmp(MatLineHeader, "map_Kd") == 0)
					fscanf(MatFile, "%s\n", &Material[MaterialCount].DiffuseTexture);

				if (strcmp(MatLineHeader, "map_Ks") == 0)
					fscanf(MatFile, "%s\n", &Material[MaterialCount].SpecularTexture);

				if (strcmp(MatLineHeader, "map_d") == 0)
					fscanf(MatFile, "%s\n", &Material[MaterialCount].AlphaTexture);

				if (strcmp(MatLineHeader, "map_bump") == 0)
					fscanf(MatFile, "%s\n", &Material[MaterialCount].NormalTexture);

				if (strcmp(MatLineHeader, "disp") == 0)
					fscanf(MatFile, "%s\n", &Material[MaterialCount].HeightTexture);

				if (strcmp(MatLineHeader, "newmtl") == 0)
				{
					MaterialCount++;
					fscanf(MatFile, "%s\n", &Material[MaterialCount].MaterialName);

					Material = (WaveModelMaterial*)realloc(Material, (MaterialCount + 2) * sizeof(WaveModelMaterial));

				}
			}

			fclose(MatFile);
		}
	NoMatterial:

		if (strcmp(LineHeader, "usemtl") == 0 && WaveHasMaterial == 1 && Settings & WAVE_LOAD_MATERIAL)
		{
			fscanf(File, "%s\n", &ObjMaterial[ObjMaterialCount].Name);
			ObjMaterial[ObjMaterialCount].Pos = Data.Mesh->VerticeCount;
			ObjMaterialCount++;
			ObjMaterial = (WaveObjMaterialDescription*)realloc(ObjMaterial, (ObjMaterialCount + 1) * sizeof(WaveObjMaterialDescription));
		}

		if (strcmp(LineHeader, "v") == 0)
		{
			fscanf(File, "%f %f %f\n", &TempVertices[TempVerticeCount].x, &TempVertices[TempVerticeCount].y, &TempVertices[TempVerticeCount].z);
			TempVerticeCount++;

			if (TempVerticeCount == TempVerticeAllocationCount - 1)
			{
				TempVerticeAllocationCount += WAVE_ALLOCATION_CHUNK_SIZE;
				TempVertices = (WaveVec3*)realloc(TempVertices, TempVerticeAllocationCount * sizeof(WaveVec3));
			}
		}

		if (strcmp(LineHeader, "vt") == 0)
		{
			fscanf(File, "%f %f\n", &TempUVs[TempUVCount].x, &TempUVs[TempUVCount].y);
			TempUVCount++;

			if (TempUVCount == TempUVAllocationCount - 1)
			{
				TempUVAllocationCount += WAVE_ALLOCATION_CHUNK_SIZE;
				TempUVs = (WaveVec3*)realloc(TempUVs, TempUVAllocationCount * sizeof(WaveVec3));
			}
		}

		if (strcmp(LineHeader, "vn") == 0)
		{
			fscanf(File, "%f %f %f\n", &TempNormals[TempNormalCount].x, &TempNormals[TempNormalCount].y, &TempNormals[TempNormalCount].z);
			TempNormalCount++;

			if (TempNormalCount == TempNormalAllocationCount - 1)
			{
				TempNormalAllocationCount += WAVE_ALLOCATION_CHUNK_SIZE;
				TempNormals = (WaveVec3*)realloc(TempNormals, TempNormalAllocationCount * sizeof(WaveVec3));
			}
		}

		if (strcmp(LineHeader, "f") == 0)
		{
			VertexReferencCount = 0;

			char ReadedLine[2048];
			fscanf(File, "%[^\n]%*c", &ReadedLine);
			char* Line = ReadedLine + 1;

			char* TempLine = strtok(Line, " ");
			while (TempLine)
			{
				int v = 0;
				int vt = 0;
				int vn = 0;

				if (TempVerticeCount > 1 && TempUVCount > 1 && TempNormalCount > 1)
					sscanf(TempLine, "%d/%d/%d", &v, &vt, &vn);

				else if (TempNormalCount == 1 && TempUVCount > 1)
					sscanf(TempLine, "%d/%d", &v, &vt);

				else if (TempUVCount == 1 && TempNormalCount > 1)
					sscanf(TempLine, "%d//%d", &v, &vn);

				else
					sscanf(TempLine, "%d", &v);

				v = (v >= 0 ? v : TempVerticeCount + v);
				vt = (vt >= 0 ? vt : TempUVCount + vt);
				vn = (vn >= 0 ? vn : TempNormalCount + vn);

				VertexReferenc[VertexReferencCount].V = v;
				VertexReferenc[VertexReferencCount].VT = vt;
				VertexReferenc[VertexReferencCount].VN = vn;

				VertexReferencCount++;
				TempLine = strtok(NULL, " ");
			}

			for (unsigned int i = 1; i + 1 < VertexReferencCount; i++)
			{
				VertexReference* p[3] = { &VertexReferenc[0], &VertexReferenc[i], &VertexReferenc[i + 1] };

				const WaveVec3 NullVec = { 0.0, 0.0, 0.0 };

				for (size_t j = 0; j < 3; j++)
				{
					Data.Mesh[Data.Mesh->VerticeCount].Vertices = TempVertices[p[j]->V];
					Data.Mesh[Data.Mesh->VerticeCount].TexCoords = TempUVs[p[j]->VT];
					Data.Mesh[Data.Mesh->VerticeCount].Normals = p[j]->VN != 0 ? TempNormals[p[j]->VN] : NullVec;
					Data.Mesh[Data.Mesh->VerticeCount].VertexColor.x = 1.0;
					Data.Mesh[Data.Mesh->VerticeCount].VertexColor.y = 1.0;
					Data.Mesh[Data.Mesh->VerticeCount].VertexColor.z = 1.0;

					if ((Settings & WAVE_GEN_UVS) && TempUVCount == 1)
						WaveGenUVs(&Data, Data.Mesh->VerticeCount);

					Data.Mesh->VerticeCount++;
					if (Data.Mesh->VerticeCount == MeshCount - 1)
					{
						MeshCount += WAVE_ALLOCATION_CHUNK_SIZE;
						Data.Mesh = (WaveMeshData*)realloc(Data.Mesh, MeshCount * sizeof(WaveMeshData));
					}
				}
			}
		}
	}

	fclose(File);

	if (WaveHasMaterial && (Settings & WAVE_LOAD_MATERIAL))
	{
		Data.HasMaterial = 1;
		Data.Material = (WaveModelMaterial*)malloc(ObjMaterialCount * sizeof(WaveModelMaterial));
		int Pos = 0;

		Material = (WaveModelMaterial*)realloc(Material, ObjMaterialCount * sizeof(WaveModelMaterial));

		for (unsigned int i = 0; i < ObjMaterialCount; i++)
		{
			for (int j = 0; j < MaterialCount + 1; j++)
			{
				if (strcmp(ObjMaterial[i].Name, Material[j].MaterialName) == 0)
				{
					Data.Material[Pos] = Material[j];
					Data.Material[Pos].VertexIndex = ObjMaterial[i].Pos;
					Pos++;
				}
			}
		}

		Data.Material->MaterialCount = ObjMaterialCount;
	}
	else
		Data.HasMaterial = 0;

	if ((Settings & WAVE_GEN_NORMALS) && TempNormalCount == 1)
		WaveGenNormals(&Data);

	//	WaveGenSmoothNormals(&Data);

	free(Material);
	free(ObjMaterial);

	free(TempVertices);
	free(TempUVs);
	free(TempNormals);

	return Data;
}

typedef struct
{
	unsigned char Header[80];
	unsigned int Triangles;
} WaveSTLDescription;

#pragma pack(push)
#pragma pack(1)

typedef struct
{
	WaveVec3 Normal;
	WaveVec3 Vertex1;
	WaveVec3 Vertex2;
	WaveVec3 Vertex3;
	short ByteCount;
} WaveSTLVertex;

#pragma pack(pop)

WaveModelData WaveLoadSTL(const char* Path, WaveSettings Settings)
{
	FILE* File = fopen(Path, "rb");
	if (!File)
		printf("Failed To Load Model!\n");

	char* Buffer;
	long Length;

	fseek(File, 0L, SEEK_END);
	Length = ftell(File);
	fseek(File, 0L, SEEK_SET);

	Buffer = (char*)malloc(Length);
	fread(Buffer, Length, 1, File);

	WaveSTLDescription* Description = (WaveSTLDescription*)Buffer;
	Buffer += sizeof(WaveSTLDescription);

	WaveSTLVertex* VertexArray = (WaveSTLVertex*)Buffer;

	unsigned int VerticeCount = Description->Triangles * 3;

	WaveModelData Data;
	Data.HasMaterial = 0;
	Data.Mesh = (WaveMeshData*)malloc(VerticeCount * sizeof(WaveMeshData));
	Data.Mesh->VerticeCount = VerticeCount;

	WaveVec3 DefaultColor = { 1.0, 1.0, 1.0 };

	unsigned int j = 0;
	for (unsigned int i = 0; i < Description->Triangles; i++)
	{
		Data.Mesh[j].Vertices = VertexArray[i].Vertex1;
		Data.Mesh[j + 1].Vertices = VertexArray[i].Vertex2;
		Data.Mesh[j + 2].Vertices = VertexArray[i].Vertex3;

		Data.Mesh[j].Normals = VertexArray[i].Normal;
		Data.Mesh[j + 1].Normals = VertexArray[i].Normal;
		Data.Mesh[j + 2].Normals = VertexArray[i].Normal;

		Data.Mesh[j].VertexColor = DefaultColor;
		Data.Mesh[j + 1].VertexColor = DefaultColor;
		Data.Mesh[j + 2].VertexColor = DefaultColor;
		if (Settings & WAVE_GEN_UVS)
		{
			WaveGenUVs(&Data, j);
			WaveGenUVs(&Data, j + 1);
			WaveGenUVs(&Data, j + 2);
		}


		j += 3;
	}

	fclose(File);
	free(Description);
	//	free(VertexArray);

	return Data;
}

typedef struct
{
	char* Line;
	unsigned int Length;

	unsigned int LineCount;
} WaveLine;

WaveLine* WaveGetLines(char* Buffer, long BufferLength)
{
	unsigned int AllocationCount = WAVE_ALLOCATION_CHUNK_SIZE;
	WaveLine* FinalLines = (WaveLine*)malloc(AllocationCount * sizeof(WaveLine));

	unsigned int LineCount = 0;

	char* p = strtok(Buffer, "\n");
	while (p)
	{
		FinalLines[LineCount].Line = p;
		FinalLines[LineCount].Length = strlen(p);

		LineCount++;

		if (LineCount == AllocationCount - 1)
		{
			AllocationCount += WAVE_ALLOCATION_CHUNK_SIZE;
			FinalLines = (WaveLine*)realloc(FinalLines, AllocationCount * sizeof(WaveLine));
		}

		p = strtok(NULL, "\n");
	}

	FinalLines->LineCount = LineCount;

	free(p);

	return FinalLines;
}

typedef struct
{
	float* FloatArray;
	unsigned int FloatArrayCount;

	unsigned int ArrayCount;
} WaveFloatArray;

typedef struct
{
	char* CharArray;
	unsigned int CharArrayCount;

	unsigned int ArrayCount;
} WaveCharArray;

WaveFloatArray GetFloatsFromString(char* Line, long Length)
{
	WaveFloatArray FinalArray;

	unsigned int AllocationCount = WAVE_ALLOCATION_CHUNK_SIZE;
	float* FloatArray = (float*)malloc(AllocationCount * sizeof(float));
	unsigned int FloatArrayCount = 0;

	char* p = strtok(Line, " ");
	while (p)
	{
		FloatArray[FloatArrayCount] = atof(p);
		FloatArrayCount++;

		if (FloatArrayCount == AllocationCount - 1)
		{
			AllocationCount += WAVE_ALLOCATION_CHUNK_SIZE;
			FloatArray = (float*)realloc(FloatArray, AllocationCount * sizeof(float));
		}

		p = strtok(NULL, " ");
	}

	FinalArray.FloatArray = FloatArray;
	FinalArray.FloatArrayCount = FloatArrayCount;
	free(p);
	return FinalArray;
}

WaveFloatArray WaveGetFloatsFromLine(char* Line, long Length)
{
	unsigned int AllocationCount = WAVE_ALLOCATION_CHUNK_SIZE;
	char* FloatLine = (char*)malloc(AllocationCount * sizeof(char));
	unsigned int FloatLineCount = 0;

	WaveFloatArray FloatArray;

	for (unsigned int i = 0; i < Length; i++)
	{
		if (Line[i] == '>')
		{
			FloatLine = (char*)malloc(AllocationCount * sizeof(char));
			FloatLineCount = 0;

			while (1)
			{
				i++;

				if (Line[i] == '<')
				{
					FloatLine[FloatLineCount] = '\0';
					FloatArray = GetFloatsFromString(FloatLine, FloatLineCount);
					break;
				}

				FloatLine[FloatLineCount] = Line[i];
				FloatLineCount++;

				if (FloatLineCount == AllocationCount - 1)
				{
					AllocationCount += WAVE_ALLOCATION_CHUNK_SIZE;
					FloatLine = (char*)realloc(FloatLine, AllocationCount * sizeof(char));
				}
			}

			free(FloatLine);
			return FloatArray;
		}
	}
}

typedef struct
{
	WaveFloatArray* FloatArray;
	unsigned int VertexPos;
	unsigned int NormalPos;
	unsigned int TexCoordPos;
	unsigned int ColorPos;
	unsigned int IndexPos;
} WaveTempMeshData;

WaveTempMeshData WaveGetMeshData(WaveLine* Lines)
{
	WaveTempMeshData MeshData;

	unsigned int FloatArrayAllocationCount = WAVE_ALLOCATION_CHUNK_SIZE;
	WaveFloatArray* FloatArray = (WaveFloatArray*)malloc(FloatArrayAllocationCount * sizeof(WaveFloatArray));
	unsigned int FloatArrayCount = 0;

	unsigned int VertexOrderAllocationCount = WAVE_ALLOCATION_CHUNK_SIZE;
	WaveCharArray* VertexOrder = (WaveCharArray*)malloc(VertexOrderAllocationCount * sizeof(WaveCharArray));
	unsigned int VertexCount = 0;

	for (unsigned int i = 0; i < Lines->LineCount; i++)
	{
		char* FoundMeshNode = strstr(Lines[i].Line, "<mesh>");
		if (FoundMeshNode)
		{
			while (1)
			{
				char* FoundMeshNodeEnd = strstr(Lines[i].Line, "</mesh>");
				if (FoundMeshNodeEnd)
					break;

				char* FoundVertex = strstr(Lines[i].Line, "float_array");

				if (FoundVertex)
				{
					FloatArray[FloatArrayCount] = WaveGetFloatsFromLine(Lines[i].Line, Lines[i].Length);
					FloatArrayCount++;

					if (FloatArrayCount == FloatArrayAllocationCount - 1)
					{
						FloatArrayAllocationCount += WAVE_ALLOCATION_CHUNK_SIZE;
						FloatArray = (WaveFloatArray*)realloc(FloatArray, FloatArrayAllocationCount * sizeof(WaveFloatArray));
					}
				}

				char* FoundIndex = strstr(Lines[i].Line, "<p>");

				if (FoundIndex)
				{
					FloatArray[FloatArrayCount] = WaveGetFloatsFromLine(Lines[i].Line, Lines[i].Length);
					FloatArrayCount++;

					if (FloatArrayCount == FloatArrayAllocationCount - 1)
					{
						FloatArrayAllocationCount += WAVE_ALLOCATION_CHUNK_SIZE;
						FloatArray = (WaveFloatArray*)realloc(FloatArray, FloatArrayAllocationCount * sizeof(WaveFloatArray));
					}
				}

				char* FoundOffset = strstr(Lines[i].Line, "offset");

				if (FoundOffset)
				{
					VertexOrder[VertexCount].CharArray = Lines[i].Line;
					VertexOrder[VertexCount].CharArrayCount = strlen(Lines[i].Line);
					VertexCount++;

					if (VertexCount == VertexOrderAllocationCount - 1)
					{
						VertexOrderAllocationCount += WAVE_ALLOCATION_CHUNK_SIZE;
						VertexOrder = (WaveCharArray*)realloc(VertexOrder, VertexOrderAllocationCount * sizeof(WaveCharArray));
					}
				}

				i++;
			}
		}
	}

	VertexOrder->ArrayCount = VertexCount;
	FloatArray->ArrayCount = FloatArrayCount;

	MeshData.FloatArray = FloatArray;

	MeshData.VertexPos = UINT32_MAX;
	MeshData.NormalPos = UINT32_MAX;
	MeshData.TexCoordPos = UINT32_MAX;
	MeshData.ColorPos = UINT32_MAX;

	for (unsigned int i = 0; i < VertexOrder->ArrayCount; i++)
	{
		char* FoundVertex = strstr(VertexOrder[i].CharArray, "VERTEX");

		if (FoundVertex)
			MeshData.VertexPos = i;

		char* FoundNormal = strstr(VertexOrder[i].CharArray, "NORMAL");

		if (FoundNormal)
			MeshData.NormalPos = i;

		char* FoundTexCoord = strstr(VertexOrder[i].CharArray, "TEXCOORD");

		if (FoundTexCoord)
			MeshData.TexCoordPos = i;

		char* FoundColor = strstr(VertexOrder[i].CharArray, "COLOR");

		if (FoundColor)
			MeshData.ColorPos = i;
	}

	free(VertexOrder);

	return MeshData;
}

WaveModelData WaveLoadDAE(const char* Path, WaveSettings Settings)
{
	char* Buffer = 0;
	long Length = 0;

	FILE* File = fopen(Path, "r");
	if (!File)
		printf("Failed To Load Model!\n");

	fseek(File, 0, SEEK_END);
	Length = ftell(File);
	fseek(File, 0, SEEK_SET);
	Buffer = (char*)malloc(Length + 1);
	fread(Buffer, sizeof(char), Length, File);

	WaveLine* Lines = WaveGetLines(Buffer, Length);
	WaveTempMeshData MeshData = WaveGetMeshData(Lines);

	unsigned int FinalVerticesAllocationCount = WAVE_ALLOCATION_CHUNK_SIZE;
	WaveVec3* FinalVertices = (WaveVec3*)malloc(FinalVerticesAllocationCount * sizeof(WaveVec3));
	unsigned int FinalNormalsAllocationCount = WAVE_ALLOCATION_CHUNK_SIZE;
	WaveVec3* FinalNormlas = (WaveVec3*)malloc(FinalNormalsAllocationCount * sizeof(WaveVec3));
	unsigned int FinalTexCoordsAllocationCount = WAVE_ALLOCATION_CHUNK_SIZE;
	WaveVec3* FinalTexCoords = (WaveVec3*)malloc(FinalTexCoordsAllocationCount * sizeof(WaveVec3));
	unsigned int FinalColorsAllocationCount = WAVE_ALLOCATION_CHUNK_SIZE;
	WaveVec3* FinalColors = (WaveVec3*)malloc(FinalColorsAllocationCount * sizeof(WaveVec3));

	unsigned int VerticesCount = 0;
	unsigned int NormalCount = 0;
	unsigned int TexCoordCount = 0;
	unsigned int ColorCount = 0;
	//Indices
	unsigned int FinalVerticeIndicesAllocationCount = WAVE_ALLOCATION_CHUNK_SIZE;
	unsigned int* FinalVerticeIndices = (unsigned int*)malloc(FinalVerticeIndicesAllocationCount * sizeof(unsigned int));
	unsigned int FinalNormalIndicesAllocationCount = WAVE_ALLOCATION_CHUNK_SIZE;
	unsigned int* FinalNormlaIndices = (unsigned int*)malloc(FinalNormalIndicesAllocationCount * sizeof(unsigned int));
	unsigned int FinalTexCoordIndicesAllocationCount = WAVE_ALLOCATION_CHUNK_SIZE;
	unsigned int* FinalTexCoordIndices = (unsigned int*)malloc(FinalTexCoordIndicesAllocationCount * sizeof(unsigned int));
	unsigned int FinalColorIndicesAllocationCount = WAVE_ALLOCATION_CHUNK_SIZE;
	unsigned int* FinalColorIndices = (unsigned int*)malloc(FinalColorIndicesAllocationCount * sizeof(unsigned int));

	unsigned int VerticesIndicesCount = 0;
	unsigned int NormalIndicesCount = 0;
	unsigned int TexCoordIndicesCount = 0;
	unsigned int ColorIndicesCount = 0;

	for (unsigned int i = 0; i < MeshData.FloatArray[MeshData.VertexPos].FloatArrayCount; i += 3)
	{
		FinalVertices[VerticesCount].x = MeshData.FloatArray[MeshData.VertexPos].FloatArray[i];
		FinalVertices[VerticesCount].y = MeshData.FloatArray[MeshData.VertexPos].FloatArray[i + 1];
		FinalVertices[VerticesCount].z = MeshData.FloatArray[MeshData.VertexPos].FloatArray[i + 2];
		VerticesCount++;

		if (VerticesCount == FinalVerticesAllocationCount - 1)
		{
			FinalVerticesAllocationCount += WAVE_ALLOCATION_CHUNK_SIZE;
			FinalVertices = (WaveVec3*)realloc(FinalVertices, FinalVerticesAllocationCount * sizeof(WaveVec3));
		}
	}

	if (MeshData.NormalPos != UINT32_MAX)
		for (unsigned int i = 0; i < MeshData.FloatArray[MeshData.NormalPos].FloatArrayCount; i += 3)
		{
			FinalNormlas[NormalCount].x = MeshData.FloatArray[MeshData.NormalPos].FloatArray[i];
			FinalNormlas[NormalCount].y = MeshData.FloatArray[MeshData.NormalPos].FloatArray[i + 1];
			FinalNormlas[NormalCount].z = MeshData.FloatArray[MeshData.NormalPos].FloatArray[i + 2];
			NormalCount++;

			if (NormalCount == FinalNormalsAllocationCount - 1)
			{
				FinalNormalsAllocationCount += WAVE_ALLOCATION_CHUNK_SIZE;
				FinalNormlas = (WaveVec3*)realloc(FinalNormlas, FinalNormalsAllocationCount * sizeof(WaveVec3));
			}
		}

	if (MeshData.TexCoordPos != UINT32_MAX)
		for (unsigned int i = 0; i < MeshData.FloatArray[MeshData.TexCoordPos].FloatArrayCount; i += 2)
		{
			FinalTexCoords[TexCoordCount].x = MeshData.FloatArray[MeshData.TexCoordPos].FloatArray[i];
			FinalTexCoords[TexCoordCount].y = MeshData.FloatArray[MeshData.TexCoordPos].FloatArray[i + 1];
			FinalTexCoords[TexCoordCount].z = 0.0;
			TexCoordCount++;

			if (TexCoordCount == FinalTexCoordsAllocationCount - 1)
			{
				FinalTexCoordsAllocationCount += WAVE_ALLOCATION_CHUNK_SIZE;
				FinalTexCoords = (WaveVec3*)realloc(FinalTexCoords, FinalTexCoordsAllocationCount * sizeof(WaveVec3));
			}
		}

	if (MeshData.ColorPos != UINT32_MAX)
		for (unsigned int i = 0; i < MeshData.FloatArray[MeshData.ColorPos].FloatArrayCount; i += 4)
		{
			FinalColors[ColorCount].x = MeshData.FloatArray[MeshData.ColorPos].FloatArray[i];
			FinalColors[ColorCount].y = MeshData.FloatArray[MeshData.ColorPos].FloatArray[i + 1];
			FinalColors[ColorCount].z = MeshData.FloatArray[MeshData.ColorPos].FloatArray[i + 2];
			ColorCount++;

			if (ColorCount == FinalColorsAllocationCount - 1)
			{
				FinalColorsAllocationCount += WAVE_ALLOCATION_CHUNK_SIZE;
				FinalColors = (WaveVec3*)realloc(FinalColors, FinalColorsAllocationCount * sizeof(WaveVec3));
			}
		}

	for (unsigned int i = 0; i < MeshData.FloatArray[MeshData.FloatArray->ArrayCount - 1].FloatArrayCount; i += MeshData.FloatArray->ArrayCount - 1)
	{
		FinalVerticeIndices[VerticesIndicesCount] = MeshData.FloatArray[MeshData.FloatArray->ArrayCount - 1].FloatArray[i + MeshData.VertexPos];
		VerticesIndicesCount++;
		if (VerticesIndicesCount == FinalVerticeIndicesAllocationCount - 1)
		{
			FinalVerticeIndicesAllocationCount += WAVE_ALLOCATION_CHUNK_SIZE;
			FinalVerticeIndices = (unsigned int*)realloc(FinalVerticeIndices, FinalVerticeIndicesAllocationCount * sizeof(unsigned int));
		}

		if (NormalCount != 0)
		{
			FinalNormlaIndices[NormalIndicesCount] = MeshData.FloatArray[MeshData.FloatArray->ArrayCount - 1].FloatArray[i + MeshData.NormalPos];
			NormalIndicesCount++;
			if (NormalIndicesCount == FinalNormalIndicesAllocationCount - 1)
			{
				FinalNormalIndicesAllocationCount += WAVE_ALLOCATION_CHUNK_SIZE;
				FinalNormlaIndices = (unsigned int*)realloc(FinalNormlaIndices, FinalNormalIndicesAllocationCount * sizeof(unsigned int));
			}
		}

		if (TexCoordCount != 0)
		{
			FinalTexCoordIndices[TexCoordIndicesCount] = MeshData.FloatArray[MeshData.FloatArray->ArrayCount - 1].FloatArray[i + MeshData.TexCoordPos];
			TexCoordIndicesCount++;
			if (TexCoordIndicesCount == FinalTexCoordIndicesAllocationCount - 1)
			{
				FinalTexCoordIndicesAllocationCount += WAVE_ALLOCATION_CHUNK_SIZE;
				FinalTexCoordIndices = (unsigned int*)realloc(FinalTexCoordIndices, FinalTexCoordIndicesAllocationCount * sizeof(unsigned int));
			}
		}

		if (ColorCount != 0)
		{
			FinalColorIndices[ColorIndicesCount] = MeshData.FloatArray[MeshData.FloatArray->ArrayCount - 1].FloatArray[i + MeshData.ColorPos];
			ColorIndicesCount++;
			if (ColorIndicesCount == FinalColorIndicesAllocationCount - 1)
			{
				FinalColorIndicesAllocationCount += WAVE_ALLOCATION_CHUNK_SIZE;
				FinalColorIndices = (unsigned int*)realloc(FinalColorIndices, FinalColorIndicesAllocationCount * sizeof(unsigned int));
			}
		}
	}

	WaveModelData Data;
	Data.HasMaterial = 0;
	Data.Mesh = (WaveMeshData*)malloc(VerticesIndicesCount * sizeof(WaveMeshData));
	Data.Mesh->VerticeCount = VerticesIndicesCount;

	for (unsigned int i = 0; i < VerticesIndicesCount; i++)
	{
		Data.Mesh[i].Vertices = FinalVertices[FinalVerticeIndices[i]];

		if (NormalIndicesCount != 0)
			Data.Mesh[i].Normals = FinalNormlas[FinalNormlaIndices[i]];

		if (TexCoordIndicesCount != 0)
		{
			if (Settings & WAVE_FLIP_UVS)
			{
				Data.Mesh[i].TexCoords.x = FinalTexCoords[FinalTexCoordIndices[i]].x;
				Data.Mesh[i].TexCoords.y = -FinalTexCoords[FinalTexCoordIndices[i]].y;
			}
			else
				Data.Mesh[i].TexCoords = FinalTexCoords[FinalTexCoordIndices[i]];
		}


		if (ColorIndicesCount != 0)
			Data.Mesh[i].VertexColor = FinalColors[FinalColorIndices[i]];
		else
		{
			Data.Mesh[i].VertexColor.x = 1.0;
			Data.Mesh[i].VertexColor.y = 1.0;
			Data.Mesh[i].VertexColor.z = 1.0;
		}

		if ((Settings & WAVE_GEN_UVS) && TexCoordIndicesCount == 0)
			WaveGenUVs(&Data, i);
		//	else
		//		Data.Material[i] = EmptyMaterial;
	}
	//	Data.Normals = WaveGenSmoothNormals(VerticesIndicesCount, Data.Vertices);
	if ((Settings & WAVE_GEN_NORMALS) && NormalIndicesCount == 0)
		WaveGenNormals(&Data);

	free(Buffer);
	free(Lines);
	free(FinalVertices);
	free(FinalNormlas);
	free(FinalTexCoords);
	free(FinalColors);

	free(FinalVerticeIndices);
	free(FinalNormlaIndices);
	free(FinalTexCoordIndices);
	free(FinalColorIndices);
	fclose(File);

	return Data;
}

WaveModelData WaveLoadWave(const char* Path, WaveSettings Settings)
{
	WaveModelData ModelData;
	
	FILE* File = fopen(Path, "rb");
	if (!File)
		return;

	unsigned int MaterialCount = 0;
	unsigned int VerticeCount = 0;

	fread(&ModelData.HasMaterial, sizeof(int), 1, File);
	fread(&MaterialCount, sizeof(int), 1, File);
	fread(&VerticeCount, sizeof(int), 1, File);

	printf("Triangles: %d\n\n", VerticeCount);

	ModelData.Material = (WaveModelMaterial*)malloc(sizeof(WaveModelMaterial) * MaterialCount);
	ModelData.Mesh = (WaveMeshData*)malloc(sizeof(WaveMeshData) * VerticeCount);

	ModelData.Material->MaterialCount = MaterialCount;
	ModelData.Mesh->VerticeCount = VerticeCount;

//	fread(&ModelData.HasMaterial, sizeof(int), 1, File);
if (ModelData.HasMaterial)
	fread(ModelData.Material, sizeof(WaveModelMaterial), ModelData.Material->MaterialCount, File);
	fread(ModelData.Mesh, sizeof(WaveMeshData), ModelData.Mesh->VerticeCount, File);

	fclose(File);

	return ModelData;
}

int WaveWriteWave(const char* Path, WaveModelData* Data)
{
	FILE* File = fopen(Path, "wb");
	if (!File)
		return 0;

	fwrite(&Data->HasMaterial, sizeof(int), 1, File);
	fwrite(&Data->Material->MaterialCount, sizeof(int), 1, File);
	fwrite(&Data->Mesh->VerticeCount, sizeof(int), 1, File);

if (Data->HasMaterial)
	fwrite(Data->Material, sizeof(WaveModelMaterial), Data->Material->MaterialCount, File);
	fwrite(Data->Mesh, sizeof(WaveMeshData), Data->Mesh->VerticeCount, File);

	fclose(File);

	return 1;
}

WaveModelData WaveLoadModel(const char* Path, WaveSettings Settings)
{
	WaveModelData ModelData;
	const char* Extension = strchr(Path, '.');

	if (strcmp(Extension + 1, "obj") == 0)
		ModelData = WaveLoadOBJ(Path, Settings);

	if (strcmp(Extension + 1, "stl") == 0)
		ModelData = WaveLoadSTL(Path, Settings);

	if (strcmp(Extension + 1, "dae") == 0)
		ModelData = WaveLoadDAE(Path, Settings);

	if (strcmp(Extension + 1, "wave") == 0)
		ModelData = WaveLoadWave(Path, Settings);

	return ModelData;
}

void WaveFreeModel(WaveModelData* ModelData)
{
	if (ModelData->HasMaterial == 1)
		free(ModelData->Material);
	free(ModelData->Mesh);
}