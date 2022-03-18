void	FragmentMain(IN(SFragmentInput) fInput, OUT(SFragmentOutput) fOutput FS_ARGS)
{
	vec3 	clipPos = fInput.fragViewProjPosition.xyz / fInput.fragViewProjPosition.w;
	vec2	fragUV0 = fInput.fragUV0;

#if defined(HAS_Atlas)
	int		blendingType = GET_CONSTANT(Material, Atlas_Blending);
	vec2	fragUV1 = fInput.fragUV1;

	float	blendMix = 0.f;

	if (blendingType == 2)
	{
		// Motion vectors
		vec2	scale = GET_CONSTANT(Material, Atlas_DistortionStrength);
		vec2	curVectors = ((SAMPLE(Atlas_MotionVectorsMap, fragUV0).rg * 2.0f) - 1.0f) * scale;
		vec2	nextVectors = ((SAMPLE(Atlas_MotionVectorsMap, fragUV1).rg * 2.0f) - 1.0f) * scale;
		float	cursor = fract(fInput.fragAtlas_TextureID);

		curVectors *= cursor;
		nextVectors *= (1.0f - cursor);

		fragUV0 = fragUV0 - curVectors;
		fragUV1 = fragUV1 + nextVectors;
		blendMix = cursor;
	}
	else if (blendingType == 1)
	{
		// Linear
		blendMix = fract(fInput.fragAtlas_TextureID);
	}
#endif // defined(HAS_Atlas)

	vec4	tex0 = SAMPLE(SixWayLightmap_RightLeftTopSmoke, fragUV0);
	vec4	tex1 = SAMPLE(SixWayLightmap_BottomBackFront, fragUV0);
#if defined(HAS_Atlas)
	if (blendingType >= 1)
	{
		vec4	tex0_1 = SAMPLE(SixWayLightmap_RightLeftTopSmoke, fragUV1);
		tex0 = mix(tex0, tex0_1, blendMix);
		vec4	tex1_1 = SAMPLE(SixWayLightmap_BottomBackFront, fragUV1);
		tex1 = mix(tex1, tex1_1, blendMix);
	}
#endif // defined(HAS_Atlas)

	vec3	lightDir = normalize((GET_CONSTANT(LightInfo, DirectionalLightsCount) < 1 ? vec3(0, 1, 0) : LOADF3(GET_RAW_BUFFER(DirectionalLightsInfo), RAW_BUFFER_INDEX(0)))).xyz;
	vec3	lightColor = (GET_CONSTANT(LightInfo, DirectionalLightsCount) < 1 ? VEC3_ZERO : LOADF3(GET_RAW_BUFFER(DirectionalLightsInfo), RAW_BUFFER_INDEX(8))).xyz;

	vec3	T = normalize(fInput.fragTangent.xyz);
	vec3	N = normalize(fInput.fragNormal.xyz);
	vec3	B = CROSS(N, T)  * fInput.fragTangent.w;
	N = fInput.IsFrontFace ? N : -N;

	vec3	rowx = vec3(T.x, B.x, N.x);
	vec3	rowy = vec3(T.y, B.y, N.y);
	vec3	rowz = vec3(T.z, B.z, N.z);

	mat3	toBBSpace = BUILD_MAT3(rowx, rowy, rowz);
	vec3	lightDirBBSpace = mul(toBBSpace, lightDir);
	
	//					   left    bottom  back          right   top    front
	vec3	tex = mix(vec3(tex0.g, tex1.r, tex1.g), vec3(tex0.r, tex0.b, tex1.b), -sign(lightDirBBSpace) * 0.5 + 0.5);
	vec3	temp = lightDirBBSpace * lightDirBBSpace * tex;
	float	outDir = temp.x + temp.y + temp.z;

	float	mipLevel = 0.7f * 6.0f;
	vec3	envRight = SAMPLELOD_CUBE(EnvironmentMapSampler, T, mipLevel).rgb;
	vec3	envLeft = SAMPLELOD_CUBE(EnvironmentMapSampler, -T, mipLevel).rgb;
	vec3	envUp = SAMPLELOD_CUBE(EnvironmentMapSampler, B, mipLevel).rgb;
	vec3	envBot = SAMPLELOD_CUBE(EnvironmentMapSampler, -B, mipLevel).rgb;
	vec3	envFw = SAMPLELOD_CUBE(EnvironmentMapSampler, N, mipLevel).rgb;
	vec3	envBw = SAMPLELOD_CUBE(EnvironmentMapSampler, -N, mipLevel).rgb;
	
	vec3	outEnv = envRight * tex0.r + envLeft * tex0.g + envUp * tex0.b + envBot * tex1.r + envBw * tex1.g + envFw * tex1.b;
	 
	vec4	color = fInput.fragSixWayLightmap_Color;
	vec4	outCol = vec4(0., 0., 0., 0.);
	float	envStr = GET_CONSTANT(Material, SixWayLightmap_EnvironmentMultiplier);
	float	dirStr = GET_CONSTANT(Material, SixWayLightmap_DirectionalMultiplier);

	outCol.rgb = color.rgb * (outEnv * envStr + outDir * dirStr * lightColor);
	outCol.a = saturate(tex0.a * color.a);
	fOutput.Output0 = outCol;
}