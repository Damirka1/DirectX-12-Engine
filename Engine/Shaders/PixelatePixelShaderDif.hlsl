
Texture2D tex;
SamplerState splr;

float4 Pixelate(float2 uv)
{
    uint2 var = uint2(uv.x * 100, uv.y * 100);
    uv = float2((float)var.x / 100, (float)var.y / 100);

    return tex.Sample(splr, uv);
}

struct PSOUT
{
    float4 Color : SV_TARGET0;
    float4 Albedo : SV_TARGET1;
    float4 Pos : SV_TARGET2;
    float4 Norm : SV_TARGET3;
};

struct VSIN
{
    float4 Pos : SV_Position;
    float3 PsPos : Positions;
    float3 Norm : NormCoord;
    float3 ViewPos: ViewPos;
    float2 Tex : Textures;
};

PSOUT main(VSIN vsin)
{
    //float3 lightPos = float3(0, 10, 0);

    float3 lightPos[] = { float3(-10, 10, -10), float3(10, 10, 10) };
    float3 lightColor = float3(1.0f, 0.8f, 0.5f);

    float3 norm = normalize(vsin.Norm);
    float3 viewDir = normalize(vsin.ViewPos - vsin.PsPos);

    float specularStrength = 0.2f;

    float3 diffuse = 0;
    float3 specular = 0;


    for (int i = 0; i < 2; i++)
    {
        float3 lightDir = normalize(lightPos[i] - vsin.PsPos);
        float3 reflectDir = reflect(-lightDir, norm);

        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32 + i * 32);
        specular += specularStrength * spec * lightColor;

        diffuse += max(dot(norm, lightDir), 0.0) * 0.1f;
    }

    float3 ambient = 0.1f * lightColor;

    float3 color = Pixelate(vsin.Tex);

    float3 result = (ambient + diffuse + specular) * color;

    PSOUT ps;

    ps.Color = float4(result, 1);
    ps.Albedo = float4(color, 1);
    ps.Pos = float4(vsin.PsPos, 1);
    ps.Norm = float4(norm, 1);

    return ps;
}