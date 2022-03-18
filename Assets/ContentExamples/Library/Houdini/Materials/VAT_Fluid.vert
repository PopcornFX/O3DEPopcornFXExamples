#if defined(HAS_VertexAnimation_Fluid)
void	VAT_Fluid(in SVertexInput vInput, inout vec3 vertPos, inout vec3 vertNormal)
{
	// Shader code based on Houdini's Game Dev Tools v1.203
	// https://github.com/sideeffects/GameDevelopmentToolset
	
	float	numFrames = GET_CONSTANT(Material, VertexAnimation_Fluid_NumFrames);
	float	cursor = ((ceil(SATURATE(vInput.VertexAnimation_Fluid_Cursor) * (numFrames - 1))) / numFrames);
	vec2	samplingUVs = vec2(vInput.UV0.x, vInput.UV0.y + cursor);
#if defined(HAS_VertexAnimation_PadToPowerOf2)
	samplingUVs *= GET_CONSTANT(Material, VertexAnimation_PadToPowerOf2_PaddedRatio);
#endif
	vec4	texPos = SAMPLELOD(VertexAnimation_Fluid_PositionMap, samplingUVs, 0);
	
#if defined(HAS_VertexAnimation_NormalizedData)
	vec2	bounds = GET_CONSTANT(Material, VertexAnimation_NormalizedData_BoundsPosition);
	texPos.xyz *= bounds.y - bounds.x;
	texPos.xyz += bounds.x;
#else
	texPos.xyz = texPos.xyz*0.01f;
#endif

	// We need to output vertex positions in user coordinate space
	vertPos = mul(GET_CONSTANT(SceneInfo, LHZToUser), vec4(texPos.xyz, 0.0)).xyz;
		
	if (GET_CONSTANT(Material, VertexAnimation_Fluid_PackedData) != 0)
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
		vec4	texNorm = SAMPLELOD(VertexAnimation_Fluid_NormalMap, samplingUVs, 0);
		vertNormal = mul(GET_CONSTANT(SceneInfo, LHZToUser), vec4(texNorm.xyz * 2.0 - 1.0, 0.0)).xyz;
	}

}
#endif // defined(HAS_VertexAnimation_Fluid)

void    VertexMain(in SVertexInput vInput, inout SVertexOutput vOutput)
{
	vec3	vertPos = vInput.Position.xyz;
	vec3	vertNormal = vInput.Normal.xyz;
	
#if defined(HAS_VertexAnimation_Fluid)
	// If fluid, texture positions are object space positions, not offsets
	vertPos = vec3(0, 0, 0);
	
	VAT_Fluid(vInput, vertPos, vertNormal);
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

	vec4	color = vec4(1,1,1,1);
#if		defined(HAS_VertexAnimation_Fluid)
	color = vInput.Diffuse_Color;
	float	numFrames = GET_CONSTANT(Material, VertexAnimation_Fluid_NumFrames);
	float	cursor = ((ceil(SATURATE(vInput.VertexAnimation_Fluid_Cursor) * (numFrames - 1))) / numFrames);
	vec2	samplingUVs = vec2(vInput.UV0.x, vInput.UV0.y + cursor);
#if defined(HAS_VertexAnimation_PadToPowerOf2)
	samplingUVs *= GET_CONSTANT(Material, VertexAnimation_PadToPowerOf2_PaddedRatio);
#endif
	vec4	texColor = SAMPLELOD(VertexAnimation_Fluid_ColorMap, samplingUVs, 0);
	color *= texColor;
#endif

	vOutput.fragWorldPosition = vec3(0, 0, 0);
	vOutput.fragViewProjPosition = vec4(0, 0, 0, 0);
	vOutput.fragUV0 = vInput.UV0;
	vOutput.fragDiffuse_Color = color;
	vOutput.fragVertexAnimation_Fluid_Cursor = vInput.VertexAnimation_Fluid_Cursor;
	
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
