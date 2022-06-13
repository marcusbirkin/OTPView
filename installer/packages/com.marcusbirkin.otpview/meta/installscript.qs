function Component()
{
	if (systemInfo.productType === "windows") { 
		installer.installationFinished.connect(this, Component.prototype.installVCRedist); 
	}
}

Component.prototype.createOperations = function()
{
	// call default implementation to actually install!
	component.createOperations();

	// Start menu links
	if (systemInfo.productType === "windows") {
		// Application
		component.addOperation("CreateShortcut", 
			"@TargetDir@/OTPView.exe",
			"@StartMenuDir@/OTPView.lnk",
			"workingDirectory=@TargetDir@",
			"iconPath=@TargetDir@/OTPView.exe",
			"iconId=0",
			"description=Start OTPView");
			
		// Uninstaller
		component.addOperation("CreateShortcut", 
			"@TargetDir@/maintenancetool.exe",
			"@StartMenuDir@/Uninstall OTPView.lnk",
			"workingDirectory=@TargetDir@",
			"iconPath=@TargetDir@/maintenancetool.exe",
			"iconId=0",
			"description=Uninstall OTPView");
	}
}

Component.prototype.installVCRedist = function()
{
	//Install Microsoft Visual C++ Redistributable
	console.log("Installing Microsoft Visual C++ Redistributable");
	var targetDir = installer.value("TargetDir");
	var msvc = QDesktopServices.findFiles(targetDir, "vc_redist.x??.exe");
	msvc.forEach((binary) => {
		console.log("Execute " + binary + " /quiet /norestart");
		installer.gainAdminRights();
		installer.execute(binary, "/quiet", "/norestart");
	});
}