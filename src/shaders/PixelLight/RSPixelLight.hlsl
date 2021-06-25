#define main "RootFlags (ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | " \
								"DENY_DOMAIN_SHADER_ROOT_ACCESS | " \
								"DENY_HULL_SHADER_ROOT_ACCESS | " \
								"DENY_GEOMETRY_SHADER_ROOT_ACCESS ), " \
			 "RootConstants(num32BitConstants = 16, b0, space=0, "\
									"visibility=SHADER_VISIBILITY_VERTEX),"\
			 "CBV(b1, space=0, visibility=SHADER_VISIBILITY_VERTEX,"\
									"flags = DATA_STATIC_WHILE_SET_AT_EXECUTE ),"\
			 "RootConstants(num32BitConstants = 15, b1, space=1, "\
									"visibility=SHADER_VISIBILITY_PIXEL),"\
			 "DescriptorTable(SRV(t0, space=0, numDescriptors=1,"\
									"flags = DESCRIPTORS_VOLATILE ),"\
									"visibility=SHADER_VISIBILITY_PIXEL),"\
			 "CBV(b0, space=1, visibility=SHADER_VISIBILITY_PIXEL,"\
									"flags = DATA_STATIC_WHILE_SET_AT_EXECUTE ),"\
			 "StaticSampler(s0,"\
							"space = 0,"\
							"filter = FILTER_MIN_MAG_MIP_LINEAR,"\
							"addressU = TEXTURE_ADDRESS_WRAP,"\
							"addressV = TEXTURE_ADDRESS_WRAP,"\
							"addressW = TEXTURE_ADDRESS_WRAP,"\
							"visibility = SHADER_VISIBILITY_PIXEL)"
