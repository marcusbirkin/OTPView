Param (
	[string]$qtframeworkdir,
	[string]$qtdir
)
If (-not($qtframeworkdir)) { Throw "Requires QT Installer Framework binaries location"}
If (-not($qtdir)) { Throw "Requires QT binaries location"}

# Stop on any error
$ErrorActionPreference = "Stop"

# https://gist.github.com/MattUebel/2292484
function Get-BinaryType {
	<#
		.SYNOPSIS
			Gets the binary executable type for a given set of files
		.DESCRIPTION
			PowerShell wrapper around the GetBinaryType Windows API that inspects file headers
			and reports the binary file type (e.g., 32-bit Windows app, 64-bit Windows app, 16-bit DOS/Windows app, etc.)
		.PARAMETER Path
			File path(s) to inspect
		.EXAMPLE
			#Reports the file type of C:\Windows\Explorer.exe:
			Get-BinaryType C:\Windows\Explorer.exe
		.EXAMPLE
			#Attempts to get the binary type of all files in the current directory
			Get-ChildItem | where { !$_.PsIsContainer } | Get-BinaryType
		.EXAMPLE
			#Attempts to get the binary type of all exe files in the windows directory,
			#ignoring any non-terminating errors
			Get-ChildItem $env:windir -filter *.exe | Get-BinaryType -ErrorAction SilentlyContinue
		.EXAMPLE
			#From a 32bit process on a 64 bit Windows install, attempts to get the binary type of all exe files 
			#in the windows system32 directory by bypassing filesystem redirection using "sysnative",
			#ignoring any non-terminating errors, and finally showing the file name and binary type
			Get-ChildItem $env:windir\sysnative -filter *.exe | Get-BinaryType -ErrorAction SilentlyContinue -passthrough | select Name,BinaryType
		.NOTES
			Author:      Battleship, Aaron Margosis
			Inspiration: http://pinvoke.net/default.aspx/kernel32/GetBinaryType.html
		.LINK
			http://wonkysoftware.appspot.com
	#>

	[CmdletBinding(
			SupportsShouldProcess   = $false,
			ConfirmImpact       = "none",
			DefaultParameterSetName = ""
	)]

	param
	(
		[Parameter(
			HelpMessage             = "Enter binary file(s) to examine",
			Position            = 0,
			Mandatory               = $true,
			ValueFromPipeline           = $true,
			ValueFromPipelineByPropertyName = $true
		)]
		[ValidateNotNullOrEmpty()]
		[ValidateScript({Test-Path $_.FullName})]
		[IO.FileInfo[]]
		$Path,

		[Alias("PassThru")]
		[switch]
		$PassThrough
	)

	begin 
	{
		try
		{
			#add the enum for the binary types
			#Using more user friendly names since they won't likely be used outside this context
			Add-Type "
				public enum BinaryType 
				{
					BIT32 = 0, // A 32-bit Windows-based application,           SCS_32BIT_BINARY
					DOS   = 1, // An MS-DOS – based application,            SCS_DOS_BINARY
					WOW   = 2, // A 16-bit Windows-based application,           SCS_WOW_BINARY
					PIF   = 3, // A PIF file that executes an MS-DOS based application, SCS_PIF_BINARY
					POSIX = 4, // A POSIX based application,                SCS_POSIX_BINARY
					OS216 = 5, // A 16-bit OS/2-based application,              SCS_OS216_BINARY
					BIT64 = 6  // A 64-bit Windows-based application,           SCS_64BIT_BINARY
				}"
		}
		catch {} #type already been loaded, do nothing

		try
		{
			# create the win32 signature
			$Signature = 
				'[DllImport("kernel32.dll")]
					public static extern bool GetBinaryType(
						string lpApplicationName,
						ref int lpBinaryType
					);
				'

			# Create a new type that lets us access the Windows API function
			Add-Type -MemberDefinition $Signature `
				-Name                 BinaryType `
				-Namespace             Win32Utils
		}
		catch {} #type already been loaded, do nothing
	}

	process 
	{
		foreach ($Item in $Path)
		{
			$ReturnedType = -1
			Write-Verbose "Attempting to get type for file: $($Item.FullName)"
			$Result = [Win32Utils.BinaryType]::GetBinaryType($Item.FullName, [ref] $ReturnedType)

			#if the function returned $false, indicating an error, or the binary type wasn't returned
			if (!$Result -or ($ReturnedType -eq -1))
			{
				Write-Error "Failed to get binary type for file $($Item.FullName)"
			}
			else
			{
				$ToReturn = [BinaryType]$ReturnedType
				if ($PassThrough) 
				{
					#get the file object, attach a property indicating the type, and passthru to pipeline
					Get-Item $Item.FullName -Force |
						Add-Member -MemberType noteproperty -Name BinaryType -Value $ToReturn -Force -PassThru 
				}
				else
				{ 
					#Put enum object directly into pipeline
					$ToReturn 
				}
			}
		}
	}
}

# Check for QT Installer Framework
Write-Host "Looking for binarycreator.exe in $qtframeworkdir"
If (-not [bool](get-command "$qtframeworkdir/binarycreator.exe").Path) {
	Write-Error "binarycreator.exe not found in $qtframeworkdir"
}

# Check for QT
Write-Host "Looking for windeployqt.exe in $qtdir"
If (-not [bool](get-command "$qtdir/windeployqt.exe").Path) {
	Write-Error "windeployqt.exe not found in $qtdir"
}

# Check for GIT
if (-not [bool](get-command git).Path) {
	Write-Error "Git not found"
}

# OTPView
$appDir = (Resolve-Path "$PSScriptRoot/..")
$appPackageDir = (Resolve-Path "$PSScriptRoot/packages/com.marcusbirkin.otpview")
$appBinary = "$appDir/build/release/OTPView.exe"

# -Copy ( and cleanup) Licence file
Get-Content "$appDir\LICENSE"  | Foreach {$_.TrimStart()}  | Foreach {$_.TrimEnd()}  | Set-Content "$appPackageDir/meta/license.txt"

# -Copy binaries
Copy-Item $appBinary -Destination "$appPackageDir/data"
& $qtdir\windeployqt.exe --no-compiler-runtime --dir "$appPackageDir/data" $appBinary
$appBit = Get-BinaryType $appBinary
Write-Host "Application bitness $appBit"
If ($appBit -eq "BIT32") {
	Write-Host "Downloading MSVC x86"
	Invoke-WebRequest https://aka.ms/vs/17/release/vc_redist.x86.exe -OutFile "$appPackageDir/data/vc_redist.x86.exe"
} ElseIf ($appBit -eq "BIT64") {
	Write-Host "Downloading MSVC x64"
	Invoke-WebRequest https://aka.ms/vs/17/release/vc_redist.x64.exe -OutFile "$appPackageDir/data/vc_redist.x64.exe"
}
Else {
	Write-Error "Unknown bitness $appBit"
}

# -Version number and release date
$appVersion = (git --git-dir="$appDir/.git" describe --always --tags)
$appDate = (git show -s --date=format:"%Y-%m-%d" --format="%cd" $appVersion)
Write-Host "OTPView Version $appVersion"
Write-Host "OTPView Release date $appDate"
$appPackage = (Resolve-Path "$PSScriptRoot/packages/com.marcusbirkin.otpview/meta/package.xml")
Write-Host "Updating $appPackage"
$appPackageXML = [xml](get-content $appPackage)
$appPackageXML.Package.Version = $appVersion
$appPackageXML.Package.ReleaseDate = $appDate
$appPackageXML.Save($appPackage)

# OTPLib
$libdir = (Resolve-Path "$PSScriptRoot/../libs/OTPLib")
$libPackageDir = (Resolve-Path "$PSScriptRoot/packages/com.marcusbirkin.otplib")
$libBinary = "$libdir/build/release/OTPLib.dll"

# -Copy ( and cleanup) Licence file
Get-Content "$libdir/LICENSE"  | Foreach {$_.TrimStart()}  | Foreach {$_.TrimEnd()}  | Set-Content "$libPackageDir/meta/license.txt"

# -Copy binaries
Copy-Item $libBinary -Destination "$libPackageDir/data"
& $qtdir\windeployqt.exe --no-compiler-runtime --dir "$libPackageDir/data" $libBinary

# -Version number and release date
$libVersion = (git --git-dir="$libdir/.git" describe --always --tags)
$libDate = (git show -s --date=format:"%Y-%m-%d" --format="%cd" $appVersion)
Write-Host "OTPLib Version $libVersion"
Write-Host "OTPLib Release date $libDate"
$libPackage = (Resolve-Path "$PSScriptRoot/packages/com.marcusbirkin.otplib/meta/package.xml")
Write-Host "Updating $libPackage"
$libPackageXML = [xml](get-content $libPackage)
$libPackageXML.Package.Version = $libVersion
$libPackageXML.Package.ReleaseDate = $libDate
$libPackageXML.Save($libPackage)

# Installer

# -Version number (Use the same as the application)
$installConfig = (Resolve-Path "$PSScriptRoot/config/config.xml")
Write-Host "Updating $installConfig"
$installConfigXML = [xml](get-content $installConfig)
$installConfigXML.Installer.Version = $appVersion
$installConfigXML.Installer.Title = "OTPView $appVersion"
If ($appBit -eq "BIT32") {
	$installConfigXML.Installer.TargetDir = "@ApplicationsDirX86@/OTPView"
	$installConfigXML.Installer.RunProgram = "@ApplicationsDirX86@/OTPView/OTPView"
} ElseIf ($appBit -eq "BIT64") {
	$installConfigXML.Installer.TargetDir = "@ApplicationsDirX64@/OTPView"
	$installConfigXML.Installer.RunProgram = "@ApplicationsDirX64@/OTPView/OTPView"
}
Else {
	$installConfigXML.Installer.TargetDir = "@ApplicationsDir@/OTPView"
	$installConfigXML.Installer.RunProgram = "@ApplicationsDir@/OTPView/OTPView"
}
$installConfigXML.Save($installConfig)

# -Create
$packagesDir = (Resolve-Path "$PSScriptRoot/packages")
$installerExe = "$PSScriptRoot/OTPView $appVersion.exe"
& $qtframeworkdir\binarycreator.exe --verbose --config "$installConfig" --packages "$packagesDir" $installerExe