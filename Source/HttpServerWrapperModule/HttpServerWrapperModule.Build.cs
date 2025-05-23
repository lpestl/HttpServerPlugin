using UnrealBuildTool;

public class HttpServerWrapperModule : ModuleRules
{
	public HttpServerWrapperModule(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"HTTPServer",
				"Json", 
				"JsonUtilities",
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"DeveloperSettings"
			}
		);
	}
}
