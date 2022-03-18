
#if defined(HAS_VertexAnimation_Rigid)
void	VAT_Rigid(in SVertexInput vInput, inout vec3 vertPos, inout vec3 vertNormal)
{
	// Shader code based on Houdini's SideFXLabs v2.1
	// https://github.com/sideeffects/SideFXLabs
	
	float	numFrames = GET_CONSTANT(Material, VertexAnimation_Rigid_NumFrames);
	float	cursor = ((ceil(SATURATE(vInput.VertexAnimation_Rigid_Cursor) * (numFrames - 1))) / numFrames);
	vec2	samplingUVs = vec2(vInput.UV1.x, vInput.UV1.y + cursor);
#if defined(HAS_VertexAnimation_PadToPowerOf2)
	samplingUVs *= GET_CONSTANT(Material, VertexAnimation_PadToPowerOf2_PaddedRatio);
#endif
	vec4	texPos = SAMPLELOD(VertexAnimation_Rigid_PositionMap, samplingUVs, 0);

#if defined(HAS_VertexAnimation_NormalizedData)
	vec2	posBounds = GET_CONSTANT(Material, VertexAnimation_NormalizedData_BoundsPosition);
	texPos.xyz *= posBounds.y - posBounds.x;
	texPos.xyz += posBounds.x;
#else
	texPos.xyz *= 0.01f;
#endif
	// Inconsistency:
	// Unity shader always applies this
	// UE4 shader applies on rgb only if "NormalizedData_1" is true, and on alpha if "NormalizedData_2"
	// PackedData defaults to true so we'll have Unity shader behavior
	vec4	texRot = SAMPLELOD(VertexAnimation_Rigid_RotationMap, samplingUVs, 0);
#if defined(HAS_VertexAnimation_NormalizedData)
		texRot *= 2.0;
		texRot -= 1.0;
#endif	
	// Vertex pivot point is stored in color stream (0)
	vec3	pivot = vInput.Color0.rgb;
	vec2	pivotBounds = GET_CONSTANT(Material, VertexAnimation_Rigid_BoundsPivot);
	pivot.xyz *= pivotBounds.y - pivotBounds.x;
	pivot.xyz += pivotBounds.x;
	pivot = mul(GET_CONSTANT(SceneInfo, LHZToUser), vec4(pivot, 0.0)).xyz;
	
	vec3	atOrigin = vertPos - pivot;
	
	vec3	quatAngle = mul(GET_CONSTANT(SceneInfo, LHZToUser), vec4(texRot.xyz, 0.0)).xyz;
	vec4	quat = vec4(quatAngle.xyz, -texRot.w * GET_CONSTANT(SceneInfo, Handedness));
	vec3	rotated = atOrigin + 2.0 * CROSS(CROSS(atOrigin, quat.xyz) + quat.w * atOrigin, quat.xyz);
	
	vertPos = rotated;
	vertPos += mul(GET_CONSTANT(SceneInfo, LHZToUser), vec4(texPos.xyz, 0.0)).xyz;
		
	vec3	rotatedNormal = vertNormal + 2.0 * CROSS(CROSS(vertNormal, quat.xyz) + quat.w * vertNormal, quat.xyz);
	vertNormal = rotatedNormal;
}
#endif // defined(HAS_VertexAnimation_Rigid)

void    VertexMain(in SVertexInput vInput, inout SVertexOutput vOutput)
{
	vec3	vertPos = vInput.Position.xyz;
	vec3	vertNormal = vInput.Normal.xyz;
	
#if defined(HAS_VertexAnimation_Rigid)
	VAT_Rigid(vInput, vertPos, vertNormal);
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
	// Ignore vertex color which contains packed pivot position
	vOutput.fragColor0 = vec4(1, 1, 1, 1);
	vOutput.fragVertexAnimation_Rigid_Cursor = vInput.VertexAnimation_Rigid_Cursor;
	
#if	defined(VOUTPUT_fragViewProjPosition)
	vOutput.fragViewProjPosition = vOutput.VertexPosition;
#endif
#if	defined(VOUTPUT_fragWorldPosition)
	vOutput.fragWorldPosition = vInput.Position;
#endif
#if	defined(VINPUT_MeshTransform)
	mat4	modelMatrix = vInput.MeshTransform;
#	if defined(VINPUT_Normal)
	vOutput.fragNormal =  mul(modelMatrix, vec4(vertNormal, 0)).xyz;
#	endif
#	if defined(VINPUT_Tagent)
	vOutput.fragTangent =  vec4(mul(modelMatrix,  vec4 (vInput.Tangent.xyz, 0)).xyz, vInput.Tangent.w);
#	endif
#endif
}
