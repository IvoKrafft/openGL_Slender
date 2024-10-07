#version 400

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;

const int MAX_LIGHTS=14;
struct Light
{
    int Type;
    vec3 Color;
    vec3 Position;
    vec3 Direction;
    vec3 Attenuation;
    vec3 SpotRadius;
    int ShadowIndex;
};

uniform Lights
{
    int LightCount;
    Light lights[MAX_LIGHTS];
};

out vec4 FragColor;

uniform vec3 LightPos;
uniform vec3 LightColor;
uniform bool enableFog; // true, wenn der Nebel aktiviert ist, andernfalls false

uniform vec3 EyePos;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform vec3 AmbientColor;
uniform float SpecularExp;
uniform sampler2D DiffuseTexture;
uniform bool RimBool;
uniform bool AuraBool;
uniform bool InvertBool;

float sat( in float a)
{
    return clamp(a, 0.0, 1.0);
}

void main()
{
    vec4 DiffTex = texture( DiffuseTexture, Texcoord);
    if(DiffTex.a <0.3f) discard;
    vec3 N = normalize(Normal);
    vec3 L = normalize(LightPos-Position);
    vec3 E = normalize(EyePos-Position);
    vec3 R = reflect(-L,N);
    vec3 DiffuseComponent;// = LightColor * DiffuseColor * sat(dot(N,L));
    vec3 H = normalize(E + L);
    vec3 SpecularComponent; //= LightColor * SpecularColor * pow( sat(dot(N,H)), SpecularExp);
    //FragColor = vec4((DiffuseComponent + AmbientColor)*DiffTex.rgb + SpecularComponent ,DiffTex.a);
    
    vec3 Color = vec3(0,0,0);
    
    for(int i = 0; i < LightCount; i++){
        float Intensity = 1.0f;
        float Dist = length(lights[i].Position - Position);
        if (lights[i].Type == 0){
            L = normalize(lights[i].Position - Position);
            Intensity /=lights[i].Attenuation.x + lights[i].Attenuation.y * Dist + lights[i].Attenuation.z * Dist * Dist;
        }else
        if (lights[i].Type == 1){
            L = normalize(-lights[i].Direction);
        }else
        if (lights[i].Type == 2){
            L = normalize(lights[i].Position - Position);
            //gleich wie bei punktlicht
            Intensity /=lights[i].Attenuation.x + lights[i].Attenuation.y * Dist + lights[i].Attenuation.z * Dist * Dist;
            //???
            float winkelL = acos(dot(normalize(L),normalize(-lights[i].Direction)));
            Intensity *= 1-sat((winkelL-lights[i].SpotRadius.x)/(lights[i].SpotRadius.y -lights[i].SpotRadius.x));
        }
        
        vec3 ColorIntensity = lights[i].Color * Intensity;
        vec3 DiffuseComponent = ColorIntensity * DiffuseColor * sat(dot(N,L));
        vec3 H = normalize(E + L);
        vec3 SpecularComponent = ColorIntensity * SpecularColor * pow(sat(dot(N,H)), SpecularExp);
        
        Color += DiffuseComponent * DiffTex.rgb + SpecularComponent;
    }
    vec3 rim = vec3(0.0f,0.0f,0.0f);
    if(RimBool){
        vec3 RimColor = vec3(0.0f,0.0f,0.5f);
        float vdn = 1.0 - max(dot(E, N), 0.0); //rim contribution
        rim=RimColor*vdn;
    }
    
    //    vec3 RimColor = vec3(0.0f,0.0f,0.5f);
    //    float vdn = 1.0 - max(dot(E, N), 0.0); //rim contribution
    //    vec3 rim=RimColor*vdn;
        //vec3 rim= vec3(smoothstep(0.8, 1.0, vdn));
        //rim.x += (RimColor.x);
        //rim.y += (RimColor.y);
        //rim.z += (RimColor.z);
  
    
    //FragColor = vec4(Color + AmbientColor*DiffTex.rgb + rim,DiffTex.a);
    //FragColor = vec4(rim,1.0);
    
    //FragColor = vec4(Color + AmbientColor*DiffTex.rgb ,DiffTex.a);

    // Aura
 
    vec3 auraColor = vec3(1.0f, 0.0f, 0.0f);
    float auraLinearStart = 1.0f;
    float auraLinearEnd = 10.0f;
    float auraDensity = 0.5f;
    float auraLength = auraLinearEnd - auraLinearStart;

    float auraDistance = length(EyePos - Position); 
  
    float auraFactor = (auraDistance - auraLinearStart) / auraLength;
   
    auraFactor = clamp(auraFactor, 0.0, 1.0);
   
    vec3 auraFinalColor = vec3(0.0f,0.0f,0.0f);

    if(AuraBool){
        auraFinalColor = mix(auraColor, Color, auraFactor);
    }
		
    

    vec3 fogColor = vec3(0.5f, 0.5f, 0.5f);
    float fogLinearStart = 60.0f;
    float fogLinearEnd = 100.0f;
    float fogLength = fogLinearEnd - fogLinearStart;

    float fogDistance = length(EyePos - Position); 
    
    float fogFactor = (fogDistance - fogLinearStart) / fogLength;
    
    fogFactor = 1.0 - clamp(fogFactor, 0.0, 1.0);

    vec3 finalColor = mix(fogColor, Color + auraFinalColor + (AmbientColor * 0.1f) * DiffTex.rgb, fogFactor);


    //FragColor = vec4(finalColor +rim ,DiffTex.a);

    if(InvertBool){
		FragColor = vec4(1.0f - (finalColor +rim) ,DiffTex.a);
	} else {
        FragColor = vec4(finalColor +rim ,DiffTex.a);
    }
    
    //FragColor = vec4(Color + AmbientColor*DiffTex.rgb,DiffTex.a);

    }
