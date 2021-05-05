// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using System;
using System.Linq;
using System.Diagnostics;
using System.Text;
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

	string ZSerializer
    {
        get
        {
			return Path.Combine(RepositoryRoot, "submodules/ZSerializer");
        }
    }

	bool BuildCore()
	{
		return ExecuteCommandSync("python tools/client_core.py --config release") == 0;
    }

	public CrazyTang(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.NoSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Voxel" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		BuildCore();
		PublicAdditionalLibraries.Add(Core);

		PublicIncludePaths.Add(RepositoryRoot);
		PublicIncludePaths.Add(Asio);
		PublicIncludePaths.Add(Eigen);
		PublicIncludePaths.Add(ZSerializer);

		bUseRTTI = true;
		bEnableExceptions = true;

		CppStandard = CppStandardVersion.Latest;

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}

    int ExecuteCommandSync(string command)
    {
        var processInfo = new ProcessStartInfo("cmd.exe", "/c " + command)
        {
            CreateNoWindow = true,
            UseShellExecute = false,
            RedirectStandardError = true,
            RedirectStandardOutput = true,
            WorkingDirectory = RepositoryRoot,
            StandardOutputEncoding = Encoding.Default
        };

        Process p = Process.Start(processInfo);
        p.OutputDataReceived += (sender, args) => Console.WriteLine(args.Data);
        p.BeginOutputReadLine();
        p.WaitForExit();

        return p.ExitCode;
    }
}
