// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class CrazyTang : ModuleRules
{
	string RepositoryRoot
    {
		get
        {
			return Path.Combine(ModuleDirectory, "../../../../");
        }
    }

	string Core
    {
        get
        {
			return Path.Combine(RepositoryRoot, "build/client_core/Release/client_core.lib");
        }
    }

	string Asio
    {
        get
        {
			return Path.Combine(RepositoryRoot, "submodules/asio/asio/include");
        }
    }

	string Eigen
    {
        get
        {
			return Path.Combine(RepositoryRoot, "submodules/eigen");
        }
    }
	

	public CrazyTang(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.NoSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicAdditionalLibraries.Add(Core);
		PublicIncludePaths.Add(RepositoryRoot);
		PublicIncludePaths.Add(Asio);
		PublicIncludePaths.Add(Eigen);

		bUseRTTI = true;
		bEnableExceptions = true;

		CppStandard = CppStandardVersion.Cpp17;

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
