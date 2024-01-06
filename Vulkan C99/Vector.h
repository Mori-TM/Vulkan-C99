typedef struct
{
	float x;
	float y;
} vec2;

typedef struct
{
	float x;
	float y;
	float z;
} vec3;

typedef struct
{
	float x;
	float y;
	float z;
	float w;
} vec4;

typedef struct
{
	float m[4][4];
} mat4;

mat4 LoadMat4Identity()
{
	mat4 Matrix;

	Matrix.m[0][0] = 1.0;
	Matrix.m[1][0] = 0.0;
	Matrix.m[2][0] = 0.0;
	Matrix.m[3][0] = 0.0;

	Matrix.m[0][1] = 0.0;
	Matrix.m[1][1] = 1.0;
	Matrix.m[2][1] = 0.0;
	Matrix.m[3][1] = 0.0;

	Matrix.m[0][2] = 0.0;
	Matrix.m[1][2] = 0.0;
	Matrix.m[2][2] = 1.0;
	Matrix.m[3][2] = 0.0;

	Matrix.m[0][3] = 0.0;
	Matrix.m[1][3] = 0.0;
	Matrix.m[2][3] = 0.0;
	Matrix.m[3][3] = 1.0;

	return Matrix;
}

mat4 MultiplyMat4(mat4 a, mat4 b)
{
	mat4 Matrix;

	Matrix.m[0][0] = a.m[0][0] * b.m[0][0] + a.m[1][0] * b.m[0][1] + a.m[2][0] * b.m[0][2] + a.m[3][0] * b.m[0][3];
	Matrix.m[0][1] = a.m[0][1] * b.m[0][0] + a.m[1][1] * b.m[0][1] + a.m[2][1] * b.m[0][2] + a.m[3][1] * b.m[0][3];
	Matrix.m[0][2] = a.m[0][2] * b.m[0][0] + a.m[1][2] * b.m[0][1] + a.m[2][2] * b.m[0][2] + a.m[3][2] * b.m[0][3];
	Matrix.m[0][3] = a.m[0][3] * b.m[0][0] + a.m[1][3] * b.m[0][1] + a.m[2][3] * b.m[0][2] + a.m[3][3] * b.m[0][3];
																	  			  			  			  
	Matrix.m[1][0] = a.m[0][0] * b.m[1][0] + a.m[1][0] * b.m[1][1] + a.m[2][0] * b.m[1][2] + a.m[3][0] * b.m[1][3];
	Matrix.m[1][1] = a.m[0][1] * b.m[1][0] + a.m[1][1] * b.m[1][1] + a.m[2][1] * b.m[1][2] + a.m[3][1] * b.m[1][3];
	Matrix.m[1][2] = a.m[0][2] * b.m[1][0] + a.m[1][2] * b.m[1][1] + a.m[2][2] * b.m[1][2] + a.m[3][2] * b.m[1][3];
	Matrix.m[1][3] = a.m[0][3] * b.m[1][0] + a.m[1][3] * b.m[1][1] + a.m[2][3] * b.m[1][2] + a.m[3][3] * b.m[1][3];
																	  			  			  			  
	Matrix.m[2][0] = a.m[0][0] * b.m[2][0] + a.m[1][0] * b.m[2][1] + a.m[2][0] * b.m[2][2] + a.m[3][0] * b.m[2][3];
	Matrix.m[2][1] = a.m[0][1] * b.m[2][0] + a.m[1][1] * b.m[2][1] + a.m[2][1] * b.m[2][2] + a.m[3][1] * b.m[2][3];
	Matrix.m[2][2] = a.m[0][2] * b.m[2][0] + a.m[1][2] * b.m[2][1] + a.m[2][2] * b.m[2][2] + a.m[3][2] * b.m[2][3];
	Matrix.m[2][3] = a.m[0][3] * b.m[2][0] + a.m[1][3] * b.m[2][1] + a.m[2][3] * b.m[2][2] + a.m[3][3] * b.m[2][3];
																	  			  			  			  
	Matrix.m[3][0] = a.m[0][0] * b.m[3][0] + a.m[1][0] * b.m[3][1] + a.m[2][0] * b.m[3][2] + a.m[3][0] * b.m[3][3];
	Matrix.m[3][1] = a.m[0][1] * b.m[3][0] + a.m[1][1] * b.m[3][1] + a.m[2][1] * b.m[3][2] + a.m[3][1] * b.m[3][3];
	Matrix.m[3][2] = a.m[0][2] * b.m[3][0] + a.m[1][2] * b.m[3][1] + a.m[2][2] * b.m[3][2] + a.m[3][2] * b.m[3][3];
	Matrix.m[3][3] = a.m[0][3] * b.m[3][0] + a.m[1][3] * b.m[3][1] + a.m[2][3] * b.m[3][2] + a.m[3][3] * b.m[3][3];


	return Matrix;
}


float determinant(mat4* a, int k)
{
	// help function for inverse_matrix
	float s = 1;
	float det = 0;
	mat4 b;
	int i, j, m, n, c;

	if (k == 1) {
		return (a->m[0][0]);
	}
	else {
		det = 0;
		for (c = 0; c < k; c++) {
			m = 0;
			n = 0;
			for (i = 0; i < k; i++) {
				for (j = 0; j < k; j++) {
					b.m[i][j] = 0;
					if (i != 0 && j != c) {
						b.m[m][n] = a->m[i][j];
						if (n < (k - 2)) {
							n++;
						}
						else {
							n = 0;
							m++;
						}
					}
				}
			}
			det = det + s * (a->m[0][c] * determinant(&b, k - 1));
			s = -1 * s;
		}
	}
	return det;
}

void cofactor(mat4* num, int f, mat4* fac)
{
	// help function for inverse_matrix
	mat4 b;
	int p, q, m, n, i, j;
	for (q = 0; q < f; q++) {
		for (p = 0; p < f; p++) {
			m = 0;
			n = 0;
			for (i = 0; i < f; i++) {
				for (j = 0; j < f; j++) {
					if (i != q && j != p) {
						b.m[m][n] = num->m[i][j];
						if (n < (f - 2)) {
							n++;
						}
						else {
							n = 0;
							m++;
						}
					}
				}
			}
			fac->m[q][p] = pow(-1.0f, (float)q + p) * determinant(&b, f - 1);
		}
	}
	// transpose(num, fac, f);
}

void transpose(mat4* num, mat4* fac, int r, mat4* inverse)
{
	// help function for inverse_matrix
	int i, j;
	mat4 b;
	float d;
	for (i = 0; i < r; i++) {
		for (j = 0; j < r; j++) {
			b.m[i][j] = fac->m[j][i];
		}
	}

	d = determinant(num, r);

	for (i = 0; i < r; i++) {
		for (j = 0; j < r; j++) {
			inverse->m[i][j] = b.m[i][j] / d;
		}
	}
}

BOOL InverseMat4(mat4* matrix, mat4* matrix_inversed)
{
	mat4 matrix_cofactored;
	cofactor(matrix, 4, &matrix_cofactored);
	transpose(matrix, &matrix_cofactored, 4, matrix_inversed);
	return TRUE;
}

vec4 MultiplyVec4Mat4(vec4 v, mat4 Matrix)
{
	vec4 r;

	r.x = v.x * Matrix.m[0][0] + v.y * Matrix.m[0][1] + v.z * Matrix.m[0][2] + v.w * Matrix.m[0][3];
	r.y = v.x * Matrix.m[1][0] + v.y * Matrix.m[1][1] + v.z * Matrix.m[1][2] + v.w * Matrix.m[1][3];
	r.z = v.x * Matrix.m[2][0] + v.y * Matrix.m[2][1] + v.z * Matrix.m[2][2] + v.w * Matrix.m[2][3];
	r.w = v.x * Matrix.m[3][0] + v.y * Matrix.m[3][1] + v.z * Matrix.m[3][2] + v.w * Matrix.m[3][3];

	return r;
}

mat4 TranslateMat4(mat4 M, float x, float y, float z)
{
	mat4 Matrix = M;

	Matrix.m[3][0] = x;
	Matrix.m[3][1] = y;
	Matrix.m[3][2] = z;
	Matrix.m[3][3] = 1.0;

	return Matrix;
}

mat4 ScaleMat4(mat4 M, float x, float y, float z)
{
	mat4 Matrix = M;

	Matrix.m[0][0] = x;
	Matrix.m[1][1] *= y;
	Matrix.m[2][2] *= z;
	Matrix.m[3][3] = 1.0;

	return Matrix;
}

mat4 TranslateMat4V(mat4 M, vec3 xyz)
{
	mat4 Matrix = M;

	Matrix.m[3][0] = xyz.x;
	Matrix.m[3][1] = xyz.y;
	Matrix.m[3][2] = xyz.z;
	Matrix.m[3][3] = 1.0;

	return Matrix;
}

mat4 ScaleMat4V(mat4 M, vec3 xyz)
{
	mat4 Matrix = M;

	Matrix.m[0][0] = xyz.x;
	Matrix.m[1][1] *= xyz.y;
	Matrix.m[2][2] *= xyz.z;
	Matrix.m[3][3] = 1.0;

	return Matrix;
}

mat4 RotateMat4(mat4 M, float angle, float x, float y, float z)
{
	mat4 Matrix;

	float Sin = sinf(angle);
	float Cos = cosf(angle);

	Matrix.m[0][0] = Cos + x * x * (1 - Cos);
	Matrix.m[0][1] = x * y * (1 - Cos) - z * Sin;
	Matrix.m[0][2] = x * z * (1 - Cos) + y * Sin;
	Matrix.m[0][3] = 0;

	Matrix.m[1][0] = y * x * (1 - Cos) + z * Sin;
	Matrix.m[1][1] = Cos + y * y * (1 - Cos);
	Matrix.m[1][2] = y * z * (1 - Cos) - x * Sin;
	Matrix.m[1][3] = 0;

	Matrix.m[2][0] = z * x * (1 - Cos) - y * Sin;
	Matrix.m[2][1] = z * y * (1 - Cos) + x * Sin;
	Matrix.m[2][2] = Cos + z * z * (1 - Cos);
	Matrix.m[2][3] = 0;

	Matrix.m[3][0] = 0;
	Matrix.m[3][1] = 0;
	Matrix.m[3][2] = 0;
	Matrix.m[3][3] = 1;

	return MultiplyMat4(Matrix, M);
}

mat4 RotateXMat4(mat4 M, float angle)
{
	mat4 Matrix;

	Matrix.m[0][0] = 1.0;
	Matrix.m[1][0] = 0.0;
	Matrix.m[2][0] = 0.0;
	Matrix.m[3][0] = 0.0;

	Matrix.m[0][1] = 0.0;
	Matrix.m[1][1] = cosf(angle);
	Matrix.m[2][1] = -sinf(angle);
	Matrix.m[3][1] = 0.0;

	Matrix.m[0][2] = 0.0;
	Matrix.m[1][2] = sinf(angle);
	Matrix.m[2][2] = cosf(angle);
	Matrix.m[3][2] = 0.0;

	Matrix.m[0][3] = 0.0;
	Matrix.m[1][3] = 0.0;
	Matrix.m[2][3] = 0.0;
	Matrix.m[3][3] = 1.0;

	return MultiplyMat4(Matrix, M);
}

mat4 RotateYMat4(mat4 M, float angle)
{

	mat4 Matrix;

	Matrix.m[0][0] = cosf(angle);
	Matrix.m[1][0] = 0.0;
	Matrix.m[2][0] = sinf(angle);
	Matrix.m[3][0] = 0.0;

	Matrix.m[0][1] = 0.0;
	Matrix.m[1][1] = 1.0;
	Matrix.m[2][1] = 0.0;
	Matrix.m[3][1] = 0.0;

	Matrix.m[0][2] = -sinf(angle);
	Matrix.m[1][2] = 0.0;
	Matrix.m[2][2] = cosf(angle);
	Matrix.m[3][2] = 0.0;

	Matrix.m[0][3] = 0.0;
	Matrix.m[1][3] = 0.0;
	Matrix.m[2][3] = 0.0;
	Matrix.m[3][3] = 1.0;

	return MultiplyMat4(Matrix, M);
}

mat4 RotateZMat4(mat4 M, float angle)
{
	mat4 Matrix;

	Matrix.m[0][0] = cosf(angle);
	Matrix.m[1][0] = -sinf(angle);
	Matrix.m[2][0] = 0.0;
	Matrix.m[3][0] = 0.0;

	Matrix.m[0][1] = sinf(angle);
	Matrix.m[1][1] = cosf(angle);
	Matrix.m[2][1] = 0.0;
	Matrix.m[3][1] = 0.0;

	Matrix.m[0][2] = 0.0;
	Matrix.m[1][2] = 0.0;
	Matrix.m[2][2] = 1.0;
	Matrix.m[3][2] = 0.0;

	Matrix.m[0][3] = 0.0;
	Matrix.m[1][3] = 0.0;
	Matrix.m[2][3] = 0.0;
	Matrix.m[3][3] = 1.0;

	return MultiplyMat4(Matrix, M);
}

float FastInvertSqrt(float number)
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

vec2 Normalize2(vec2 v)
{
	float length = FastInvertSqrt(v.x * v.x + v.y * v.y);
	v.x *= length;
	v.y *= length;

	return v;
}

vec3 Normalize3(vec3 v)
{
	float length = FastInvertSqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	v.x *= length;
	v.y *= length;
	v.z *= length;

	return v;
}

vec4 Normalize4(vec4 v)
{
	float length = FastInvertSqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	v.x *= length;
	v.y *= length;
	v.z *= length;
	v.w *= length;

	return v;
}

vec3 Cross(vec3 a, vec3 b)
{
	vec3 r;
	r.x = a.y * b.z - a.z * b.y;
	r.y = a.z * b.x - a.x * b.z;
	r.z = a.x * b.y - a.y * b.x;

	return r;
}

float Dot(vec3 a, vec3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec2 Add2(vec2 a, vec2 b)
{
	vec2 r;
	r.x = a.x + b.x;
	r.y = a.y + b.y;

	return r;
}

vec2 Sub2(vec2 a, vec2 b)
{
	vec2 r;
	r.x = a.x - b.x;
	r.y = a.y - b.y;

	return r;
}

vec2 Mul2(vec2 a, vec2 b)
{
	vec2 r;
	r.x = a.x * b.x;
	r.y = a.y * b.y;

	return r;
}

vec2 Div2(vec2 a, vec2 b)
{
	vec2 r;
	r.x = a.x / b.x;
	r.y = a.y / b.y;

	return r;
}

vec3 Add3(vec3 a, vec3 b)
{
	vec3 r;
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;

	return r;
}

vec3 Sub3(vec3 a, vec3 b)
{
	vec3 r;
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	r.z = a.z - b.z;

	return r;
}

vec3 Mul3(vec3 a, vec3 b)
{
	vec3 r;
	r.x = a.x * b.x;
	r.y = a.y * b.y;
	r.z = a.z * b.z;

	return r;
}

vec3 Div3(vec3 a, vec3 b)
{
	vec3 r;
	r.x = a.x / b.x;
	r.y = a.y / b.y;
	r.z = a.z / b.z;

	return r;
}

vec4 Add4(vec4 a, vec4 b)
{
	vec4 r;
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;
	r.w = a.w + b.w;

	return r;
}

vec4 Sub4(vec4 a, vec4 b)
{
	vec4 r;
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	r.z = a.z - b.z;
	r.w = a.w - b.w;

	return r;
}

vec4 Mul4(vec4 a, vec4 b)
{
	vec4 r;
	r.x = a.x * b.x;
	r.y = a.y * b.y;
	r.z = a.z * b.z;
	r.w = a.w * b.w;

	return r;
}

vec4 Div4(vec4 a, vec4 b)
{
	vec4 r;
	r.x = a.x / b.x;
	r.y = a.y / b.y;
	r.z = a.z / b.z;
	r.w = a.w / b.w;

	return r;
}

float Lenght3(vec3 v)
{
	return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

vec2 Vec2(float x, float y)
{
	vec2 r = { x, y };
	return r;
}

vec3 Vec3(float x, float y, float z)
{
	vec3 r = { x, y, z };
	return r;
}

vec4 Vec4(float x, float y, float z, float w)
{
	vec4 r = { x, y, z, w };
	return r;
}

mat4 LookAt(vec3 Pos, vec3 LookAt, vec3 Up)
{
	mat4 Matrix;
	
	vec3 Z = Normalize3(Sub3(LookAt, Pos));
	vec3 X = Normalize3(Cross(Z, Up));
	vec3 Y = Cross(X, Z);

	Z.x *= -1;
	Z.y *= -1;
	Z.z *= -1;

	Matrix.m[0][0] = X.x;
	Matrix.m[1][0] = X.y;
	Matrix.m[2][0] = X.z;
	Matrix.m[3][0] = -Dot(X, Pos);
	Matrix.m[0][1] = Y.x;
	Matrix.m[1][1] = Y.y;
	Matrix.m[2][1] = Y.z;
	Matrix.m[3][1] = -Dot(Y, Pos);
	Matrix.m[0][2] = Z.x;
	Matrix.m[1][2] = Z.y;
	Matrix.m[2][2] = Z.z;
	Matrix.m[3][2] = -Dot(Z, Pos);
	Matrix.m[0][3] = 0;
	Matrix.m[1][3] = 0;
	Matrix.m[2][3] = 0;
	Matrix.m[3][3] = 1.0;

	return Matrix;
}

float Radians(float Angle)
{
	return Angle * 3.14159265359 / 180;
}

mat4 PerspectivMatrix(float Aspect, float FOV, float NearPlane, float FarPlane)
{
	float F = 1.0f / tanf(Radians(0.5 * FOV));

	mat4 Matrix =
	{
	  F / Aspect,
	  0.0,
	  0.0,
	  0.0,

	  0.0,
	  -F,
	  0.0,
	  0.0,

	  0.0,
	  0.0,
	  FarPlane / (NearPlane - FarPlane),
	  -1.0,

	  0.0,
	  0.0,
	  (NearPlane * FarPlane) / (NearPlane - FarPlane),
	  0.0
	};

	return Matrix;
}

mat4 OrthoMatrix(float Left, float Right, float Bottom, float Top, float NearZ, float FarZ)
{
	mat4 Matrix;
	Matrix.m[0][0] = 2.0 / (Right - Left);
	Matrix.m[1][0] = 0.0;
	Matrix.m[2][0] = 0.0;
	Matrix.m[3][0] = (Left + Right) / (Left - Right);

	Matrix.m[0][1] = 0.0;
	Matrix.m[1][1] = 2.0 / (Top - Bottom);
	Matrix.m[2][1] = 0.0;
	Matrix.m[3][1] = (Bottom + Top) / (Bottom - Top);

	Matrix.m[0][2] = 0.0;
	Matrix.m[1][2] = 0.0;
	Matrix.m[2][2] = -2.0 / (FarZ - NearZ);
	Matrix.m[3][2] = (FarZ + NearZ) / (NearZ - FarZ);

	Matrix.m[0][3] = 0.0;
	Matrix.m[1][3] = 0.0;
	Matrix.m[2][3] = 0.0;
	Matrix.m[3][3] = 1.0;

	/*
	mat4 Matrix;
	Matrix.m[0][0] = 2.0f / (Right - Left);
	Matrix.m[1][0] = 0.0;
	Matrix.m[2][0] = 0.0;
	Matrix.m[3][0] = (Left + Right) / (Left - Right);

	Matrix.m[0][1] = 0.0;
	Matrix.m[1][1] = 2.0f / (Top - Bottom);
	Matrix.m[2][1] = 0.0;
	Matrix.m[3][1] = (Bottom + Top) / (Bottom - Top);

	Matrix.m[0][2] = 0.0;
	Matrix.m[1][2] = 0.0;
	Matrix.m[2][2] = 1.0;
	Matrix.m[3][2] = 0.0;

	Matrix.m[0][3] = 0.0;
	Matrix.m[1][3] = 0.0;
	Matrix.m[2][3] = 0.0;
	Matrix.m[3][3] = 1.0;
	*/
	return Matrix;
}

float GetDistanceBetween2Points2(vec2 Pos1, vec2 Pos2)
{
	float DX = Pos1.x - Pos2.x;
	float DY = Pos1.y - Pos2.y;

	DX *= DX;
	DY *= DY;

	DX = sqrtf(DX);
	DY = sqrtf(DY);

	return DX + DY;
}

float GetDistanceBetween2Points3(vec3 Pos1, vec3 Pos2)
{
	float DX = Pos1.x - Pos2.x;
	float DY = Pos1.y - Pos2.y;
	float DZ = Pos1.z - Pos2.z;

	DX *= DX;
	DY *= DY;
	DZ *= DZ;

	DX = sqrtf(DX);
	DY = sqrtf(DY);
	DZ = sqrtf(DZ);

	return DX + DY + DZ;
}

int RandomInt(int Min, int Max)
{
	return (rand() % (Max - Min + 1)) + Min;
}

void printVec2(vec2 a)
{
	printf("%f %f\n", a.x, a.y);
}

void printVec3(vec3 a)
{
	printf("%f %f %f\n", a.x, a.y, a.z);
}

void printVec4(vec4 a)
{
	printf("%f %f %f %f\n", a.x, a.y, a.z, a.w);
}