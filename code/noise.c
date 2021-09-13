
typedef struct noise
{
    u8 perm[512];
} noise;

global const noise Noise_Default = {
    .perm = {
        151,160,137, 91, 90, 15,131, 13,201, 95, 96, 53,194,233,  7,225,
        140, 36,103, 30, 69,142,  8, 99, 37,240, 21, 10, 23,190,  6,148,
        247,120,234, 75,  0, 26,197, 62, 94,252,219,203,117, 35, 11, 32,
         57,177, 33, 88,237,149, 56, 87,174, 20,125,136,171,168, 68,175,
         74,165, 71,134,139, 48, 27,166, 77,146,158,231, 83,111,229,122,
         60,211,133,230,220,105, 92, 41, 55, 46,245, 40,244,102,143, 54,
         65, 25, 63,161,  1,216, 80, 73,209, 76,132,187,208, 89, 18,169,
        200,196,135,130,116,188,159, 86,164,100,109,198,173,186,  3, 64,
         52,217,226,250,124,123,  5,202, 38,147,118,126,255, 82, 85,212,
        207,206, 59,227, 47, 16, 58, 17,182,189, 28, 42,223,183,170,213,
        119,248,152,  2, 44,154,163, 70,221,153,101,155,167, 43,172,  9,
        129, 22, 39,253, 19, 98,108,110, 79,113,224,232,178,185,112,104,
        218,246, 97,228,251, 34,242,193,238,210,144, 12,191,179,162,241,
         81, 51,145,235,249, 14,239,107, 49,192,214, 31,181,199,106,157,
        184, 84,204,176,115,121, 50, 45,127,  4,150,254,138,236,205, 93,
        222,114, 67, 29, 24, 72,243,141,128,195, 78, 66,215, 61,156,180,

        151,160,137, 91, 90, 15,131, 13,201, 95, 96, 53,194,233,  7,225,
        140, 36,103, 30, 69,142,  8, 99, 37,240, 21, 10, 23,190,  6,148,
        247,120,234, 75,  0, 26,197, 62, 94,252,219,203,117, 35, 11, 32,
         57,177, 33, 88,237,149, 56, 87,174, 20,125,136,171,168, 68,175,
         74,165, 71,134,139, 48, 27,166, 77,146,158,231, 83,111,229,122,
         60,211,133,230,220,105, 92, 41, 55, 46,245, 40,244,102,143, 54,
         65, 25, 63,161,  1,216, 80, 73,209, 76,132,187,208, 89, 18,169,
        200,196,135,130,116,188,159, 86,164,100,109,198,173,186,  3, 64,
         52,217,226,250,124,123,  5,202, 38,147,118,126,255, 82, 85,212,
        207,206, 59,227, 47, 16, 58, 17,182,189, 28, 42,223,183,170,213,
        119,248,152,  2, 44,154,163, 70,221,153,101,155,167, 43,172,  9,
        129, 22, 39,253, 19, 98,108,110, 79,113,224,232,178,185,112,104,
        218,246, 97,228,251, 34,242,193,238,210,144, 12,191,179,162,241,
         81, 51,145,235,249, 14,239,107, 49,192,214, 31,181,199,106,157,
        184, 84,204,176,115,121, 50, 45,127,  4,150,254,138,236,205, 93,
        222,114, 67, 29, 24, 72,243,141,128,195, 78, 66,215, 61,156,180
    }
};
#if 0
global const vec3 Noise_grad3lut[16] = {
  { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f },
  {-1.0f, 0.0f, 1.0f }, { 0.0f,-1.0f, 1.0f },
  { 1.0f, 0.0f,-1.0f }, { 0.0f, 1.0f,-1.0f },
  {-1.0f, 0.0f,-1.0f }, { 0.0f,-1.0f,-1.0f },
  { 1.0f,-1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f },
  {-1.0f, 1.0f, 0.0f }, {-1.0f,-1.0f, 0.0f },

  { 1.0f, 0.0f, 1.0f }, {-1.0f, 0.0f, 1.0f },
  { 0.0f, 1.0f,-1.0f }, { 0.0f,-1.0f,-1.0f }
};

global const vec4 Noise_grad4lut[32] = {
  { 0.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 1.0f,-1.0f }, { 0.0f, 1.0f,-1.0f, 1.0f }, { 0.0f, 1.0f,-1.0f,-1.0f },
  { 0.0f,-1.0f, 1.0f, 1.0f }, { 0.0f,-1.0f, 1.0f,-1.0f }, { 0.0f,-1.0f,-1.0f, 1.0f }, { 0.0f,-1.0f,-1.0f,-1.0f },
  { 1.0f, 0.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f,-1.0f }, { 1.0f, 0.0f,-1.0f, 1.0f }, { 1.0f, 0.0f,-1.0f,-1.0f },
  {-1.0f, 0.0f, 1.0f, 1.0f }, {-1.0f, 0.0f, 1.0f,-1.0f }, {-1.0f, 0.0f,-1.0f, 1.0f }, {-1.0f, 0.0f,-1.0f,-1.0f },
  { 1.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f,-1.0f }, { 1.0f,-1.0f, 0.0f, 1.0f }, { 1.0f,-1.0f, 0.0f,-1.0f },
  {-1.0f, 1.0f, 0.0f, 1.0f }, {-1.0f, 1.0f, 0.0f,-1.0f }, {-1.0f,-1.0f, 0.0f, 1.0f }, {-1.0f,-1.0f, 0.0f,-1.0f },
  { 1.0f, 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f,-1.0f, 0.0f }, { 1.0f,-1.0f, 1.0f, 0.0f }, { 1.0f,-1.0f,-1.0f, 0.0f },
  {-1.0f, 1.0f, 1.0f, 0.0f }, {-1.0f, 1.0f,-1.0f, 0.0f }, {-1.0f,-1.0f, 1.0f, 0.0f }, {-1.0f,-1.0f,-1.0f, 0.0f }
};

global const u8 Noise_simplex[64][4] = {
    {0,1,2,3},{0,1,3,2},{0,0,0,0},{0,2,3,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,2,3,0},
    {0,2,1,3},{0,0,0,0},{0,3,1,2},{0,3,2,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,3,2,0},
    {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
    {1,2,0,3},{0,0,0,0},{1,3,0,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,3,0,1},{2,3,1,0},
    {1,0,2,3},{1,0,3,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,0,3,1},{0,0,0,0},{2,1,3,0},
    {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
    {2,0,1,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,0,1,2},{3,0,2,1},{0,0,0,0},{3,1,2,0},
    {2,1,0,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,1,0,2},{0,0,0,0},{3,2,0,1},{3,2,1,0}
};
#endif
inline f32 Noise_grad1(u32 hash)
{
    u32 h = hash & 15;
    f32 grad = 1.0f + (h & 7);
    if (h & 8) grad = -grad;
    return grad;
}

global const vec2 Noise_grad2[8] = {
  {-1.0f,-1.0f }, { 1.0f, 0.0f }, {-1.0f, 0.0f }, { 1.0f, 1.0f },
  {-1.0f, 1.0f }, { 0.0f,-1.0f }, { 0.0f, 1.0f }, { 1.0f,-1.0f }
};

#if 0
inline f32 Noise_grad3(u32 hash, vec3 p)
{
    u32 h = hash & 15;
    vec3 g = Noise_grad3lut[h];
    return Vec3_Dot(p, g);
}

inline f32 Noise_grad4(u32 hash, vec4 p)
{
    u32 h = hash & 31;
    vec4 g = Noise_grad4lut[h];
    return Vec4_Dot(p, g);
}
#endif

f32 Noise_Simplex1D(const noise *Noise, f32 x, f32 *grad)
{
    const f32 SCALE = 0.25f;

    const u8 *perm = Noise_Default.perm;
    if (Noise) perm = Noise->perm;

    f32 xi = F32_Floor(x);
    f32 x0 = x - xi;
    f32 x1 = x0 - 1.0f;

    i32 i0 = F32_FloatToI32(xi);
    i32 i1 = i0 + 1;

    f32 x02 = (x0 * x0);
    f32 t0 = 1.0f - x02;
    f32 t02 = t0 * t0;
    f32 t04 = t02 * t02;
    f32 g0 = Noise_grad1(perm[i0 & 0xFF]);
    f32 n0 = t04 * x0 * g0;

    f32 x12 = (x1 * x1);
    f32 t1 = 1.0f - x12;
    f32 t12 = t1 * t1;
    f32 t14 = t12 * t12;
    f32 g1 = Noise_grad1(perm[i1 & 0xFF]);
    f32 n1 = t14 * x1 * g1;

    if (grad)
    {
        f32 g = t02 * t0 * g0 * x02;
            g += t12 * t1 * g1 * x12;
            g *= -8.0f;
            g += t04 * g0 + t14 * g1;
            g *= SCALE;
        *grad = g;
    }

    return SCALE * (n0 + n1);
}

f32 Noise_Simplex2D(const noise *Noise, vec2 p, vec2 *grad)
{
    const f32 SCALE = 40.0f;
    const f32 F2 = 0.36602540378f;
    const f32 G2 = 0.2113248654f;

    const u8 *perm = Noise_Default.perm;
    if (Noise) perm = Noise->perm;

    // skew to regular grid coordinates
    vec2 s = Vec2_Set1(Vec2_Sum(p) * F2);
    vec2 ps = Floor(Vec2_Add(p, s));

    // simplex indices
    u8 i = F32_FloatToI32(ps.x) & 0xFF;
    u8 j = F32_FloatToI32(ps.y) & 0xFF;

    // unskew to simplex grid coordinates
    vec2 u = Vec2_Set1(Vec2_Sum(ps) * G2);
    vec2 pu = Vec2_Sub(ps, u);
    
    // positions relative to simplex vertices 
    vec2 p0 = Vec2_Sub(p, pu);
    u8 i1 = (p0.x > p0.y);
    u8 j1 = i1 ^ 1;
    vec2 ij1 = (vec2){ i1, j1 };
    vec2 p1 = Vec2_Add(Vec2_Sub(p0, ij1), Vec2_Set1(G2));
    vec2 p2 = Vec2_Add(p0, Vec2_Set1(-1.0f + 2.0f * G2));

    // gradients
    vec2 g0 = Noise_grad2[perm[i+   perm[j   ]] & 7];
    vec2 g1 = Noise_grad2[perm[i+i1+perm[j+j1]] & 7];
    vec2 g2 = Noise_grad2[perm[i+ 1+perm[j+ 1]] & 7];

    // values
    f32 v0 = Vec2_Dot(g0, p0);
    f32 v1 = Vec2_Dot(g1, p1);
    f32 v2 = Vec2_Dot(g2, p2);

    // interpolation values
    f32 t0 = Max(0.0f, 0.5f - Vec2_LengthSq(p0));
    f32 t0_2 = t0 * t0;
    f32 t0_4 = t0_2 * t0_2;

    f32 t1 = Max(0.0f, 0.5f - Vec2_LengthSq(p1));
    f32 t1_2 = t1 * t1;
    f32 t1_4 = t1_2 * t1_2;

    f32 t2 = Max(0.0f, 0.5f - Vec2_LengthSq(p2));
    f32 t2_2 = t2 * t2;
    f32 t2_4 = t2_2 * t2_2;

    // simplex vertex contributions
    f32 n0 = t0_4 * v0;
    f32 n1 = t1_4 * v1;
    f32 n2 = t2_4 * v2;

    // derivatives
    // f32 dn0_dx  = (-8.0f * x0 * t0_3 * v0) + (t0_4 * g0.x);
    // f32 dn0_dy  = (-8.0f * y0 * t0_3 * v0) + (t0_4 * g0.y);
    // f32 dn1_dx += (-8.0f * x1 * t0_3 * v1) + (t1_4 * g1.x);
    // f32 dn1_dy += (-8.0f * y1 * t0_3 * v1) + (t1_4 * g1.y);
    // f32 dn2_dx += (-8.0f * x2 * t0_3 * v2) + (t2_4 * g2.x);
    // f32 dn3_dx += (-8.0f * y2 * t0_3 * v2) + (t2_4 * g2.y);

    // derivatives
    if(grad)
    {
        vec2 g =        Vec2_Mul(p0, Vec2_Set1(t0_2 * t0 * v0));
        g = Vec2_Add(g, Vec2_Mul(p1, Vec2_Set1(t1_2 * t1 * v1)));
        g = Vec2_Add(g, Vec2_Mul(p2, Vec2_Set1(t2_2 * t2 * v2)));
        g = Vec2_Mul(g, Vec2_Set1(-8.0f));
        g = Vec2_Add(g, Vec2_Mul(g0, Vec2_Set1(t0_4)));
        g = Vec2_Add(g, Vec2_Mul(g1, Vec2_Set1(t1_4)));
        g = Vec2_Add(g, Vec2_Mul(g2, Vec2_Set1(t2_4)));
        g = Vec2_Mul(g, Vec2_Set1(SCALE));
        *grad = g;
    }

    return SCALE * (n0 + n1 + n2);
}

#if 0

f32 Noise_Simplex3D(vec3 p)
{
    const f32 F3 = 0.333333333f;
    const f32 G3 = 0.166666667f;

    f32 x = p.x;
    f32 y = p.y;
    f32 z = p.z;

    f32 s = (x + y + z) * F3;
    f32 xs = x + s;
    f32 ys = y + s;
    f32 zs = z + s;
    i32 i = F32_FloorToI32(xs);
    i32 j = F32_FloorToI32(ys);
    i32 k = F32_FloorToI32(zs);

    f32 t = (f32)(i + j + k) * G3;
    f32 X0 = i - t;
    f32 Y0 = j - t;
    f32 Z0 = k - t;
    f32 x0 = x - X0;
    f32 y0 = y - Y0;
    f32 z0 = z - Z0;

    i32 i1, j1, k1;
    i32 i2, j2, k2;

    /* This code would benefit from a backport from the GLSL version! */
    if(x0 >= y0)
    {
        if (y0>=z0){i1=1; j1=0; k1=0; i2=1; j2=1; k2=0;} // X Y Z order
        else if (x0>=z0) { i1=1; j1=0; k1=0; i2=1; j2=0; k2=1; } // X Z Y order
        else { i1=0; j1=0; k1=1; i2=1; j2=0; k2=1; } // Z X Y order
    }
    else
    {
        if (y0<z0) { i1=0; j1=0; k1=1; i2=0; j2=1; k2=1; } // Z Y X order
        else if (x0<z0) { i1=0; j1=1; k1=0; i2=0; j2=1; k2=1; } // Y Z X order
        else { i1=0; j1=1; k1=0; i2=1; j2=1; k2=0; } // Y X Z order
    }

    f32 x1 = x0 - i1 + G3;
    f32 y1 = y0 - j1 + G3;
    f32 z1 = z0 - k1 + G3;
    f32 x2 = x0 - i2 + 2.0f * G3;
    f32 y2 = y0 - j2 + 2.0f * G3;
    f32 z2 = z0 - k2 + 2.0f * G3;
    f32 x3 = x0 - 1.0f + 3.0f * G3;
    f32 y3 = y0 - 1.0f + 3.0f * G3;
    f32 z3 = z0 - 1.0f + 3.0f * G3;

    u8 ii = i & 0xFF;
    u8 jj = j & 0xFF;
    u8 kk = k & 0xFF;

    f32 n0 = 0.0f;
    f32 t0 = 0.6f - x0*x0 - y0*y0 - z0*z0;
    if (t0 >= 0.0f)
    {
        f32 t02 = t0 * t0;
        f32 t04 = t02 * t02;
        n0 = t04 * grad3(perm[ii+perm[jj+perm[kk]]], x0, y0, z0);
    }

    f32 n1 = 0.0f;
    f32 t1 = 0.6f - x1*x1 - y1*y1 - z1*z1;
    if (t1 >= 0.0f)
    {
        f32 t12 = t1 * t1;
        f32 t14 = t12 * t12;
        n1 = t14 * grad3(perm[ii+i1+perm[jj+j1+perm[kk+k1]]], x1, y1, z1);
    }

    f32 n2 = 0.0f;
    f32 t2 = 0.6f - x2*x2 - y2*y2 - z2*z2;
    if (t2 >= 0.0f)
    {
        f32 t22 = t2 * t2;
        f32 t24 = t22 * t22;
        n2 = t24 * grad3(perm[ii+i2+perm[jj+j2+perm[kk+k2]]], x2, y2, z2);
    }

    f32 n3 = 0.0f;
    f32 t3 = 0.6f - x3*x3 - y3*y3 - z3*z3;
    if (t3 >= 0.0f)
    {
        f32 t32 = t3 * t3;
        f32 t34 = t32 * t32;
        n3 = t34 * grad3(perm[ii+1+perm[jj+1+perm[kk+1]]], x3, y3, z3);
    }

    return 28.0f * (n0 + n1 + n2 + n3);
}


f32 Noise_Simplex4D(f32 x, f32 y, f32 z, f32 w)
{
    const f32 F4 = 0.309016994f
    const f32 G4 = 0.138196601f

    f32 s = (x + y + z + w) * F4;
    f32 xs = x + s;
    f32 ys = y + s;
    f32 zs = z + s;
    f32 ws = w + s;
    i32 i = F32_FloorToInt(xs);
    i32 j = F32_FloorToInt(ys);
    i32 k = F32_FloorToInt(zs);
    i32 l = F32_FloorToInt(ws);

    f32 t = (i + j + k + l) * G4;
    f32 X0 = i - t;
    f32 Y0 = j - t;
    f32 Z0 = k - t;
    f32 W0 = l - t;

    f32 x0 = x - X0;
    f32 y0 = y - Y0;
    f32 z0 = z - Z0;
    f32 w0 = w - W0;

    i32 c1 = (x0 > y0) ? 32 : 0;
    i32 c2 = (x0 > z0) ? 16 : 0;
    i32 c3 = (y0 > z0) ? 8 : 0;
    i32 c4 = (x0 > w0) ? 4 : 0;
    i32 c5 = (y0 > w0) ? 2 : 0;
    i32 c6 = (z0 > w0) ? 1 : 0;
    i32 c = c1 | c2 | c3 | c4 | c5 | c6;

    i32 i1 = Noise_simplex[c][0]>=3 ? 1 : 0;
    i32 j1 = Noise_simplex[c][1]>=3 ? 1 : 0;
    i32 k1 = Noise_simplex[c][2]>=3 ? 1 : 0;
    i32 l1 = Noise_simplex[c][3]>=3 ? 1 : 0;
    i32 i2 = Noise_simplex[c][0]>=2 ? 1 : 0;
    i32 j2 = Noise_simplex[c][1]>=2 ? 1 : 0;
    i32 k2 = Noise_simplex[c][2]>=2 ? 1 : 0;
    i32 l2 = Noise_simplex[c][3]>=2 ? 1 : 0;
    i32 i3 = Noise_simplex[c][0]>=1 ? 1 : 0;
    i32 j3 = Noise_simplex[c][1]>=1 ? 1 : 0;
    i32 k3 = Noise_simplex[c][2]>=1 ? 1 : 0;
    i32 l3 = Noise_simplex[c][3]>=1 ? 1 : 0;

    f32 x1 = x0 - i1 + G4;
    f32 y1 = y0 - j1 + G4;
    f32 z1 = z0 - k1 + G4;
    f32 w1 = w0 - l1 + G4;
    f32 x2 = x0 - i2 + 2.0f * G4;
    f32 y2 = y0 - j2 + 2.0f * G4;
    f32 z2 = z0 - k2 + 2.0f * G4;
    f32 w2 = w0 - l2 + 2.0f * G4;
    f32 x3 = x0 - i3 + 3.0f * G4;
    f32 y3 = y0 - j3 + 3.0f * G4;
    f32 z3 = z0 - k3 + 3.0f * G4;
    f32 w3 = w0 - l3 + 3.0f * G4;
    f32 x4 = x0 - 1.0f + 4.0f * G4;
    f32 y4 = y0 - 1.0f + 4.0f * G4;
    f32 z4 = z0 - 1.0f + 4.0f * G4;
    f32 w4 = w0 - 1.0f + 4.0f * G4;

    u8 ii = i & 0xFF;
    u8 jj = j & 0xFF;
    u8 kk = k & 0xFF;
    u8 ll = l & 0xFF;

    f32 n0 = 0.0f;
    f32 t0 = 0.6f - x0*x0 - y0*y0 - z0*z0 - w0*w0;
    if (t0 >= 0.0f)
    {
        f32 t02 = t0 * t0;
        f32 t04 = t02 * t02;
        n0 = t04 * grad4(perm[ii+perm[jj+perm[kk+perm[ll]]]], x0, y0, z0, w0);
    }

    f32 n1 = 0.0f;
    f32 t1 = 0.6f - x1*x1 - y1*y1 - z1*z1 - w1*w1;
    if (t1 >= 0.0f)
    {
        f32 t12 = t1 * t1;
        f32 t14 = t12 * t12;
        n1 = t14 * grad4(perm[ii+i1+perm[jj+j1+perm[kk+k1+perm[ll+l1]]]], x1, y1, z1, w1);
    }

    f32 n2 = 0.0f;
    f32 t2 = 0.6f - x2*x2 - y2*y2 - z2*z2 - w2*w2;
    if (t2 >= 0.0f)
    {
        f32 t22 = t2 * t2;
        f32 t24 = t22 * t22;
        n2 = t24 * grad4(perm[ii+i2+perm[jj+j2+perm[kk+k2+perm[ll+l2]]]], x2, y2, z2, w2);
    }

    f32 n3 = 0.0f;
    f32 t3 = 0.6f - x3*x3 - y3*y3 - z3*z3 - w3*w3;
    if (t3 >= 0.0f)
    {
        f32 t32 = t3 * t3;
        f32 t34 = t32 * t32;
        n3 = t34 * grad4(perm[ii+i3+perm[jj+j3+perm[kk+k3+perm[ll+l3]]]], x3, y3, z3, w3);
    }

    f32 n4 = 0.0f;
    f32 t4 = 0.6f - x4*x4 - y4*y4 - z4*z4 - w4*w4;
    if (t4 >= 0.0f)
    {
        f32 t42 = t4 * t4;
        f32 t44 = t42 * t42;
        n4 = t44 * t44 * grad4(perm[ii+1+perm[jj+1+perm[kk+1+perm[ll+1]]]], x4, y4, z4, w4);
    }

    return 27.0f * (n0 + n1 + n2 + n3 + n4);
}
#endif

f32 Noise_FBM2D(const noise *Noise, vec2 Position, u32 Octaves)
{
    f32 Result = 0.0f;
    f32 Amplitude = 0.5f;

    for (u32 i = 0; i < Octaves; ++i)
    {
        Result += Amplitude * Noise_Simplex2D(Noise, Position, 0);
        Position = Vec2_Mul(Position, Vec2_Set1(2.0f));
        Amplitude *= 0.5f;
    }
    return Result;
}

noise Noise_Random(rng *Rng)
{
    noise Noise;

    for (u32 i = 0; i < 256; ++i)
        Noise.perm[i] = i & 0xFF;
    for (u32 i = 255; i > 0; --i)
    {
        u8 j = (Rng_U64(Rng) % (i + 1)) & 0xFF;
        u8 Temp = Noise.perm[j];
        Noise.perm[j] = Noise.perm[i];
        Noise.perm[i] = Temp;
    }
    for (u32 i = 0; i < 256; ++i)
        Noise.perm[i+256] = Noise.perm[i];

    return Noise;
}

typedef struct voronoi_result
{
    f32 PointDist;
    f32 BorderDist;
    ivec2 PointCell;
    ivec2 BorderCell;
} voronoi_result;

f32 Noise_Voronoi(vec2 P, voronoi_result *Result)
{
    ivec2 N = Vec2_FloorToIVec2(P);
    vec2 F = Vec2_Fract(P);

    f32 MinDist = 8.0f;
    vec2 MinPoint = { 0 };
    ivec2 MinCell = { 0 };
    for (i32 x = -2; x <= 2; ++x)
    for (i32 y = -2; y <= 2; ++y)
    {
        ivec2 LocalCell = (ivec2){ .x = x, .y = y };
        ivec2 Cell = iVec2_Add(N, LocalCell);
        vec2 Offset = Hash_toVec2(Hash_IVec2(Cell));
        vec2 Point = Vec2_Sub(Vec2_Add(iVec2_toVec2(LocalCell), Offset), F);

        f32 Dist = Vec2_Length(Point);
        if (Dist < MinDist)
        {
            MinDist = Dist;
            MinPoint = Point;
            MinCell = Cell;
        }
    }

    if (Result)
    {
        f32 MinBorderDist = 8.0f;
        ivec2 MinBorderCell = { 0 };
        for (i32 x = -2; x <= 2; ++x)
        for (i32 y = -2; y <= 2; ++y)
        {
            ivec2 LocalCell = (ivec2){ .x = x, .y = y };
            ivec2 Cell = iVec2_Add(N, LocalCell);
            vec2 Offset = Hash_toVec2(Hash_IVec2(Cell));
            vec2 Point = Vec2_Sub(Vec2_Add(iVec2_toVec2(LocalCell), Offset), F);

            vec2 Border = Vec2_Mul(Vec2_Add(Point, MinPoint), Vec2_Set1(0.5f));
            f32 BorderDist = Vec2_Dot(Border, Vec2_Normalize(Vec2_Sub(Point, MinPoint)));
            if (BorderDist < MinBorderDist)
            {
                MinBorderDist = BorderDist;
                MinBorderCell = Cell;
            }
        }

        *Result = (voronoi_result) {
            .PointDist = MinDist,
            .BorderDist = MinBorderDist,
            .PointCell = MinCell,
            .BorderCell = MinBorderCell
        };
    }

    return MinDist;
}
