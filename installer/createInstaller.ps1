Param (
	[string]$qtframeworkdir,
	[string]$qtdir
)
If (-not($qtframeworkdir)) { Throw "Requires QT Installer Framework location" }
If (-not($qtdir)) {
	If (Test-Path -Path Env:Qt6_DIR) {
		$qtdir = (Get-ChildItem -Path Env:Qt6_DIR).Value
	}
	ElseIf (Test-Path -Path Env:Qt5_DIR) {
		$qtdir = (Get-ChildItem -Path Env:Qt5_DIR).Value
	}
	Else {
		Throw "Requires QT location or QT5_DIR/QT6_DIR enviroment variable"
	}
}

$qtdir = (Resolve-Path "$qtdir")
$qtframeworkdir = (Resolve-Path "$qtframeworkdir")

# Stop on any error
$ErrorActionPreference = "Stop"

# https://gist.github.com/MattUebel/2292484
Function Get-BinaryType {
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
		SupportsShouldProcess = $false,
		ConfirmImpact = "none",
		DefaultParameterSetName = ""
	)]

	param
	(
		[Parameter(
			HelpMessage = "Enter binary file(s) to examine",
			Position = 0,
			Mandatory = $true,
			ValueFromPipeline = $true,
			ValueFromPipelineByPropertyName = $true
		)]
		[ValidateNotNullOrEmpty()]
		[ValidateScript({ Test-Path $_.FullName })]
		[IO.FileInfo[]]
		$Path,

		[Alias("PassThru")]
		[switch]
		$PassThrough
	)

	begin {
		try {
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

		try {
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

	process {
		foreach ($Item in $Path) {
			$ReturnedType = -1
			Write-Verbose "Attempting to get type for file: $($Item.FullName)"
			$Result = [Win32Utils.BinaryType]::GetBinaryType($Item.FullName, [ref] $ReturnedType)

			#if the function returned $false, indicating an error, or the binary type wasn't returned
			if (!$Result -or ($ReturnedType -eq -1)) {
				Write-Error "Failed to get binary type for file $($Item.FullName)"
			}
			else {
				$ToReturn = [BinaryType]$ReturnedType
				if ($PassThrough) {
					#get the file object, attach a property indicating the type, and passthru to pipeline
					Get-Item $Item.FullName -Force |
					Add-Member -MemberType noteproperty -Name BinaryType -Value $ToReturn -Force -PassThru
				}
				else {
					#Put enum object directly into pipeline
					$ToReturn
				}
			}
		}
	}
}

# Check for QT Installer Framework
# @return full path to binarcycreator
Function checkQtInstallerBinaryCreator {
	If ($IsLinux) {
		$binarycreator = "binarycreator"
	}
	ElseIf ($IsMacOS) {
		Write-Error "Unsupported OS"
		Exit 1
	}
	ElseIf ($IsWindows) {
		$binarycreator = "binarycreator.exe"
	}

	Write-Host -NoNewline "Looking for $binarycreator in $qtframeworkdir..."
	Try {
		$ret = (get-command "$qtframeworkdir/bin/$binarycreator").Path
	}
	Catch {
		Write-Error "Not Found"
		Exit 1
	}
	Write-Host "Found"
	Return $ret
}

# Check for deployqt and download if appropriate (Linux)
# @return full path to deployqt
Function checkDeployQt {
	If ($IsLinux) {
		Write-Host -NoNewline "Looking for linuxdeployqt in $PSScriptRoot..."
		try {
			$ret = (get-command "$PSScriptRoot/linuxdeploy-x86_64.AppImage").Path
		}
		catch {
			Write-Host -NoNewline "Not Found, downloading..."
			Push-Location (Resolve-Path "$PSScriptRoot")
			Invoke-WebRequest `
				"https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage" `
				-OutFile linuxdeploy-x86_64.AppImage
			Invoke-WebRequest `
				"https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage" `
				-OutFile linuxdeploy-plugin-qt-x86_64.AppImage
			chmod +x linuxdeploy*.AppImage
			Pop-Location
		}

		Try {
			$ret = (get-command "$PSScriptRoot/linuxdeploy-x86_64.AppImage").Path
		}
		Catch {
			Write-Error "Not Found"
			Exit 1
		}
		Write-Host "Found"
		Return $ret
	}
	ElseIf ($IsMacOS) {
		Write-Error "Unsupported OS"
		Exit 1
	}
	ElseIf ($IsWindows) {
		Write-Host -NoNewline "Looking for windeployqt.exe in $qtdir..."
		try {
			$ret = (get-command "$qtdir/bin/windeployqt.exe").Path
		}
		catch {
			Write-Error "Not Found"
			Exit 1
		}
		Write-Host "Found"
		Return $ret
	}
}

# Check system has QMake
# @return Full path to QMake
Function checkQMake {
	If ($IsLinux) {
		$qmake = "qmake"
	}
	ElseIf ($IsMacOS) {
		Write-Error "Unsupported OS"
		Exit 1
	}
	ElseIf ($IsWindows) {
		$qmake = "qmake.exe"
	}

	Write-Host -NoNewline "Looking for $qmake in $qtdir..."
	Try {
		$ret = (get-command "$qtdir/bin/$qmake").Path
	}
	Catch {
		Write-Error "Not Found"
		Exit 1
	}
	Write-Host "Found"
	Return $ret
}

# Check system has Git
# @return Full path to Git
Function checkGit {
	Write-Host -NoNewline "Looking for Git..."
	Try {
		$ret = (get-command git).Path
	}
	Catch {
		Write-Error "Not Found"
		Exit 1
	}
	Write-Host "Found"
	Return $ret
}

# Setup OTPLib in package directory
Function setupApplication {
	Write-Host "Setting up OTPView..."

	$appDir = (Resolve-Path "$PSScriptRoot/..")
	$appPackageDir = (Resolve-Path "$PSScriptRoot/packages/com.marcusbirkin.otpview")
	$appPackageBinaryDir = "$appPackageDir/data"
	$appBinaryDir = "$appDir/build/release"

	If ($IsLinux) {
		$appBinary = "OTPView"
		$appIcon = (Resolve-Path "$appDir/app/res/icon.png")
	}
	ElseIf ($IsMacOS) {
		Write-Error "Unsupported OS"
		Exit 1
	}
	ElseIf ($IsWindows) {
		$appBinary = "OTPView.exe"
		$appBit = Get-BinaryType "$appBinaryDir/$appBinary"
		$global:appBit = $appBit
	}

	# Copy (and cleanup) Licence file
	Get-Content "$appDir\LICENSE" | ForEach-Object { $_.TrimStart() }  | ForEach-Object { $_.TrimEnd() }  | Set-Content "$appPackageDir/meta/license.txt"

	# Copy binaries and dependencies
	If ($IsLinux) {
		write-host $Env:LD_LIBRARY_PATH
		$Env:LD_LIBRARY_PATH = "$qtdir/lib:$Env:LD_LIBRARY_PATH"
		write-host $Env:LD_LIBRARY_PATH
		$Env:QMAKE = checkQMake
		$deployqt = checkDeployQt
		& $deployqt --appdir $appPackageBinaryDir `
			--executable="$appBinaryDir/$appBinary" `
			--exclude-library=*OTPLib* `
			--plugin=qt `
			--icon-file="$appicon" --icon-filename="OTPView" `
			--create-desktop-file

		if ($LASTEXITCODE -ne 0) {
			Write-Error "Deployment failed"
			Exit 1
		}
	}
	ElseIf ($IsMacOS) {
		Write-Error "Unsupported OS"
		Exit 1
	}
	ElseIf ($IsWindows) {
		Copy-Item "$appBinaryDir/$appBinary" -Destination "$appPackageBinaryDir"
		$deployqt = checkDeployQt
		& $deployqt --no-compiler-runtime --dir $appPackageBinaryDir "$appBinaryDir/$appBinary"
		if ($LASTEXITCODE -ne 0) {
			Write-Error "Deployment failed"
			Exit 1
		}
	}

	# Version number and release date
	$appVersion = (git --git-dir="$appDir/.git" describe --always --tags)
	$global:appVersion = $appVersion
	$appDate = (git --git-dir="$appDir/.git" show -s --date=format:"%Y-%m-%d" --format="%cd" $appVersion)
	Write-Host "OTPView Version $appVersion"
	Write-Host "OTPView Release date $appDate"
	$appPackage = (Resolve-Path "$appPackageDir/meta/package.xml")
	Write-Host "Updating $appPackage"
	$appPackageXML = [xml](get-content $appPackage)
	$appPackageXML.Package.Version = $appVersion
	$appPackageXML.Package.ReleaseDate = $appDate
	$appPackageXML.Save($appPackage)
}

# Setup OTPLib in package directory
Function setupLibrary {
	Write-Host "Setting up OTPLib..."

	$libdir = (Resolve-Path "$PSScriptRoot/../libs/OTPLib")
	$libPackageDir = (Resolve-Path "$PSScriptRoot/packages/com.marcusbirkin.otplib")
	$libBinaryDir = "$libdir/build/release"
	If ($IsLinux) {
		$libBinary = "libOTPLib.so*"
		$libPackageBinaryDir = "$libPackageDir/data/usr/lib"
		$Env:LD_LIBRARY_PATH = "$libPackageBinaryDir"
	}
	ElseIf ($IsMacOS) {
		Write-Error "Unsupported OS"
		Exit 1
	}
	ElseIf ($IsWindows) {
		$libBinary = "OTPLib.dll"
		$libPackageBinaryDir = "$libPackageDir/data"
	}

	# Copy (and cleanup) Licence file
	Get-Content "$libdir/LICENSE" | ForEach-Object { $_.TrimStart() }  | ForEach-Object { $_.TrimEnd() }  | Set-Content "$libPackageDir/meta/license.txt"

	# Copy binaries
	If (!(Test-Path $libPackageBinaryDir)) {
		Write-Host "Creating folder $libPackageBinaryDir"
		[void](New-Item -Type Directory -Path $libPackageBinaryDir)
	}
	Write-Host "Copying $libBinaryDir/$libBinary to $libPackageBinaryDir"
	Copy-Item $libBinaryDir/$libBinary -Destination "$libPackageBinaryDir"

	# Version number and release date
	$libVersion = (git --git-dir="$libdir/.git" describe --always --tags)
	$libDate = (git --git-dir="$libdir/.git" show -s --date=format:"%Y-%m-%d" --format="%cd" $libVersion)
	Write-Host "OTPLib Version $libVersion"
	Write-Host "OTPLib Release date $libDate"
	$libPackage = (Resolve-Path "$libPackageDir/meta/package.xml")
	Write-Host "Updating $libPackage"
	$libPackageXML = [xml](get-content $libPackage)
	$libPackageXML.Package.Version = $libVersion
	$libPackageXML.Package.ReleaseDate = $libDate
	$libPackageXML.Save($libPackage)
}

# Setup MSVC in package directory
Function setupMSVC {
	Write-Host "Setting up Microsoft Visual C++ Redistributable..."

	$msvcPackageDir = (Resolve-Path "$PSScriptRoot/packages/com.microsoft.vcredist")
	$msvcPackageBinaryDir = "$msvcPackageDir/data"

	Write-Host "Application bitness $appBit"
	If ($global:appBit -eq "BIT32") {
		$msvcBinary = "vc_redist.x86.exe"
	}
	ElseIf ($global:appBit -eq "BIT64") {

		$msvcBinary = "vc_redist.x64.exe"
	}
	Else {
		Write-Error "Unknown bitness $appBit"
		Exit 1
	}

	If (!(Test-Path $msvcPackageBinaryDir)) {
		Write-Host "Creating folder $msvcPackageBinaryDir"
		[void](New-Item -Type Directory -Path $msvcPackageBinaryDir)
	}

	Write-Host "Downloading Microsoft Visual C++ Redistributable $msvcBinary"
	Invoke-WebRequest "https://aka.ms/vs/17/release/$msvcBinary" -OutFile "$msvcPackageBinaryDir/$msvcBinary"

	# Version number and release date
	$msvcVersion = (Get-Item "$msvcPackageBinaryDir/$msvcBinary").VersionInfo.ProductVersion
	$msvcDate = ((Get-AuthenticodeSignature "$msvcPackageBinaryDir/$msvcBinary").TimeStamperCertificate.NotBefore).toString(“yyyy-MM-dd”)
	Write-Host "MSVC Version $msvcVersion"
	Write-Host "MSVC Release date $msvcDate"
	$msvcPackage = (Resolve-Path "$msvcPackageDir/meta/package.xml")
	Write-Host "Updating $msvcPackage"
	$msvcPackageXML = [xml](get-content $msvcPackage)
	$msvcPackageXML.Package.Version = $msvcVersion
	$msvcPackageXML.Package.ReleaseDate = $msvcDate
	$msvcPackageXML.Save($msvcPackage)
}

# Create the actual installer
Function createInstaller {
	# Version number (Use the same as the application)
	$installConfig = (Resolve-Path "$PSScriptRoot/config/config.xml")
	Write-Host "Updating $installConfig"
	$installConfigXML = [xml](get-content $installConfig)
	$installConfigXML.Installer.Version = $global:appVersion
	$installConfigXML.Installer.Title = "OTPView $appVersion"

	# Destination folder and post install run
	$installConfigXML.Installer.TargetDir = "@ApplicationsDir@/OTPView"
	$installConfigXML.Installer.RunProgram = "@TargetDir@/OTPView"
	If ($IsLinux) {
		$installConfigXML.Installer.RunProgram = "@TargetDir@/AppRun"
	}
	ElseIf ($IsMacOS) {
		Write-Error "Unsupported OS"
		Exit 1
	}
	ElseIf ($IsWindows) {
		If ($global:appBit -eq "BIT32") {
			$installConfigXML.Installer.TargetDir = "@ApplicationsDirX86@/OTPView"
		}
		ElseIf ($global:appBit -eq "BIT64") {
			$installConfigXML.Installer.TargetDir = "@ApplicationsDirX64@/OTPView"
		}
	}

	# Save config.xml
	$installConfigXML.Save($installConfig)

	# Create
	$packagesDir = (Resolve-Path "$PSScriptRoot/packages")
	$installerBinary = "$PSScriptRoot/OTPView $appVersion"
	If ($IsLinux) {
		$installerBinary = "$installerBinary (Linux $(uname -m))"
	}
	ElseIf ($IsMacOS) {
		$installerBinary = "$installerBinary (macOS $(uname -m)).app"
	}
	ElseIf ($IsWindows) {
		If ($global:appBit -eq "BIT32") {
			$installerBinary = "$installerBinary (Windows x86).exe"
		}
		ElseIf ($global:appBit -eq "BIT64") {
			$installerBinary = "$installerBinary (Windows x64).exe"
		}
	}

	$binarycreator = checkQtInstallerBinaryCreator
	& $binarycreator --verbose --config "$installConfig" --packages "$packagesDir" "$installerBinary"
}

# Check prerequisites
[void](checkQtInstallerBinaryCreator)
[void](checkQMake)
[void](checkDeployQt)
[void](checkGit)

# Setup packages for installer
setupLibrary
setupApplication
If ($IsWindows) {
	setupMSVC
}

# Create the installer
createInstaller
