#define main "RootFlags (ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | " \
								"DENY_DOMAIN_SHADER_ROOT_ACCESS | " \
								"DENY_HULL_SHADER_ROOT_ACCESS | " \
								"DENY_GEOMETRY_SHADER_ROOT_ACCESS ), " \
			 "RootConstants(num32BitConstants = 16, b0, space=0, "\
									"visibility=SHADER_VISIBILITY_VERTEX),"\
			 "CBV(b0, space=1, visibility=SHADER_VISIBILITY_PIXEL,"\
									"flags = DATA_STATIC_WHILE_SET_AT_EXECUTE ),"\
			 "CBV(b1, space=1, visibility=SHADER_VISIBILITY_PIXEL,"\
									"flags = DATA_STATIC ),"\
			 "CBV(b1, space=0, visibility=SHADER_VISIBILITY_VERTEX,"\
									"flags = DATA_STATIC_WHILE_SET_AT_EXECUTE )"\

