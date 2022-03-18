#if defined(HAS_VertexAnimation_Soft)
void	VAT_Soft(in SVertexInput vInput, inout vec3 vertPos, inout vec3 vertNormal)
{
	// Shader code based on Houdini's SideFXLabs v2.1
	// https://github.com/sideeffects/SideFXLabs
	
	float	numFrames = GET_CONSTANT(Material, VertexAnimation_Soft_NumFrames);
	float	cursor = ((ceil(SATURATE(vInput.VertexAnimation_Soft_Cursor) * (numFrames - 1))) / numFrames);
	vec2	samplingUVs = vec2(vInput.UV1.x, vInput.UV1.y + cursor);
#if defined(HAS_VertexAnimation_PadToPowerOf2)
	samplingUVs *= GET_CONSTANT(Material, VertexAnimation_PadToPowerOf2_PaddedRatio);
#endif
	vec4	texPos = SAMPLELOD(VertexAnimation_Soft_PositionMap, samplingUVs, 0);
	
#if defined(HAS_VertexAnimation_NormalizedData)
	vec2	bounds = GET_CONSTANT(Material, VertexAnimation_NormalizedData_BoundsPosition);
	texPos.xyz *= bounds.y - bounds.x;
	texPos.xyz += bounds.x;
#else
	texPos *= 0.01f;
#endif
	// We need to output vertex positions in user coordinate space
	vertPos += mul(GET_CONSTANT(SceneInfo, LHZToUser), vec4(texPos.xyz, 0.0)).xyz;
		
	if (GET_CONSTANT(Material, VertexAnimation_Soft_PackedData) != 0)
	{
		// Houdini's float -> float3 decode
		float	alpha = texPos.w * 1024;
		float	x = floor(alpha / 32.0) / 31.5;
		float	y = (alpha - (floor(alpha / 32.0) * 32.0)) / 31.5;
		vec2	f2 = vec2(x, y) * 4 - 2;
			
		float	f2dot = dot(f2,f2);
		vec3	f3;
		f3.xy = sqrt(1 - (f2dot / 4.0)) * f2;
		f3.z = 1 - (f2dot / 2.0);
		f3 = clamp(f3, -1.0, 1.0);
		
		vertNormal = mul(GET_CONSTANT(SceneInfo, LHZToUser), vec4(f3, 0.0)).xyz;
	}
	else
	{
		// If we have no normal map available, keep our vertex normal
		vec4	texNorm = SAMPLELOD(VertexAnimation_Soft_NormalMap, samplingUVs, 0);
		vertNormal = mul(GET_CONSTANT(SceneInfo, LHZToUser), vec4(texNorm.xyz * 2.0 - 1.0, 0.0)).xyz;
	}
}
#endif // defined(HAS_VertexAnimation_Soft)

void    VertexMain(in SVertexInput vInput, inout SVertexOutput vOutput)
{
	vec3	vertPos = vInput.Position.xyz;
	vec3	vertNormal = vInput.Normal.xyz;
	
#if defined(HAS_VertexAnimation_Soft)
	VAT_Soft(vInput, vertPos, vertNormal);
#endif

#if		defined(CONST_SceneInfo_ViewProj)
#	if		defined(VINPUT_MeshTransform)
	vec4	pos = mul(vInput.MeshTransform, vec4(vertPos, 1.0));
	vOutput.VertexPosition = mul(GET_CONSTANT(SceneInfo, ViewProj), pos);
#	else
	vOutput.VertexPosition = mul(GET_CONSTANT(SceneInfo, ViewProj), vec4(vertPos, 1.0));
#	endif
#else
	vOutput.VertexPosition = vec4(vertPos, 1.0);
#endif

	vOutput.fragWorldPosition = vec3(0, 0, 0);
	vOutput.fragViewProjPosition = vec4(0, 0, 0, 0);
	vOutput.fragUV0 = vInput.UV0;
	vOutput.fragUV1 = vInput.UV1;
	vOutput.fragDiffuse_Color = vInput.Diffuse_Color;
	vOutput.fragVertexAnimation_Soft_Cursor = vInput.VertexAnimation_Soft_Cursor;
	
#if	defined(VOUTPUT_fragViewProjPosition)
	vOutput.fragViewProjPosition = vOutput.VertexPosition;
#endif
#if	defined(VOUTPUT_fragWorldPosition)
	vOutput.fragWorldPosition = vInput.Position;
#endif
#if	defined(VINPUT_MeshTransform)
	mat4	modelMatrix = vInput.MeshTransform;
#	if defined(VINPUT_Normal)
	vOutput.fragNormal = mul(modelMatrix, vec4(vertNormal, 0)).xyz;
#	endif
#	if defined(VINPUT_Tagent)
	vOutput.fragTangent =  vec4 (mul(modelMatrix,  vec4 (vInput.Tangent.xyz, 0)).xyz, vInput.Tangent.w);
#	endif
#endif
}
