#if 	defined(HAS_TransformUVs)
vec2 transformUV(vec2 UV, vec2 scale, mat2 rotation, vec2 offset)
{
	return mul(rotation, UV * scale) + offset;
}
#endif

#if defined(HAS_SoftParticles)
float   clipToLinearDepth(float depth FS_ARGS)
{
	float zNear = GET_CONSTANT(SceneInfo, ZBufferLimits).x;
	float zFar = GET_CONSTANT(SceneInfo, ZBufferLimits).y;
	return (-zNear * zFar) / (depth * (zFar - zNear) - zFar);
}
#endif

void    FragmentMain(IN(SFragmentInput) fInput, OUT(SFragmentOutput) fOutput FS_ARGS)
{
	vec4    color = vec4(0,0,0,0);

#if     defined(FINPUT_fragDiffuse_Color)
	color = fInput.fragDiffuse_Color;
#else
	color = vec4(1.0f, 1.0f, 1.0f, 0.1f);
#endif

#if	    defined(FINPUT_fragColor0)
	color *= fInput.fragColor0;
#endif

vec2 fragUV0 = fInput.fragUV0;
#if		defined(HAS_TransformUVs)
	float	sinR = sin(fInput.fragTransformUVs_UVRotate);
	float	cosR = cos(fInput.fragTransformUVs_UVRotate);
	mat2	UVRotation = mat2(cosR, sinR, -sinR, cosR);
	vec2	UVScale = fInput.fragTransformUVs_UVScale;
	vec2	UVOffset = fInput.fragTransformUVs_UVOffset;
	vec2	oldFragUV0 = fragUV0;	
	fragUV0 = transformUV(fragUV0, UVScale, UVRotation, UVOffset); // scale then rotate then translate UV
	fragUV0 = fract(fragUV0);
	bool	RGBOnly = GET_CONSTANT(Material, TransformUVs_RGBOnly) != 0;
#endif

#if	defined(HAS_Diffuse)
	vec4    textureColor = SAMPLE(Diffuse_DiffuseMap, fragUV0);
	
#	if defined(HAS_TransformUVs)
		if (RGBOnly)
		{
			textureColor.a =  SAMPLE(Diffuse_DiffuseMap, oldFragUV0).a;
		}
#	endif

#if defined(HAS_DiffuseRamp)
	textureColor.rgb = SAMPLE(DiffuseRamp_RampMap, vec2(textureColor.x,0.0)).rgb;
#endif

	color *= textureColor;

#if	defined(HAS_AlphaRemap)
	vec2    alphaTexCoord = vec2(color.a, fInput.fragAlphaRemap_Cursor);
	color.a = SAMPLE(AlphaRemap_AlphaMap, alphaTexCoord).r;
#endif 

#endif // defined HAS_Diffuse

#if	defined(HAS_SoftParticles)
	vec4    projPos = fInput.fragViewProjPosition;
	float   rcpw = 1.0 / projPos.w;
	float   fragDepth_cs = projPos.z * rcpw;
	//  if (IsDepthReversed())
	//      fragDepth_cs = 1.0 - fragDepth_cs;
	vec2    screenUV = projPos.xy * rcpw * vec2(0.5, 0.5) + 0.5;
	float   sceneDepth_cs = SAMPLE(DepthSampler, screenUV).x;
	float   sceneDepth = clipToLinearDepth(sceneDepth_cs FS_PARAMS);
	float   fragDepth = clipToLinearDepth(fragDepth_cs FS_PARAMS);
	float   invSoftnessDistance = 1.0f / GET_CONSTANT(Material, SoftParticles_SoftnessDistance);
	float   depthfade = clamp((sceneDepth - fragDepth) * invSoftnessDistance, 0.f, 1.f);
	color *= depthfade;
#endif

#if	defined(HAS_Emissive)
	vec3	emissiveColor1 = SAMPLE(Emissive_EmissiveMap, fragUV0).rgb;	
#if	defined(HAS_EmissiveRamp)
	emissiveColor1 = SAMPLE(EmissiveRamp_RampMap, vec2(emissiveColor1.x,0.0)).rgb;
#endif
	emissiveColor1 *= fInput.fragEmissive_EmissiveColor;
	color.rgb += emissiveColor1.rgb;
#endif

	fOutput.Output0 = color;
}
