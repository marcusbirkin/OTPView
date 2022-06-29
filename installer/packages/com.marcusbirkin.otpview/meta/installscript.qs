function Component(){}

Component.prototype.createOperations = function()
{
	// call default implementation to actually install!
	component.createOperations();

	// Start menu links
	if (systemInfo.kernelType === "winnt") {
		// Windows

		// Main Application
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
	} else if (systemInfo.kernelType === "darwin") {
		// macOS (Future Support)
	} else {
		// Linux
		component.addElevatedOperation("Execute", 
			"desktop-file-edit", "@TargetDir@/OTPView.desktop", 
			"--set-key=Exec", "--set-value=@TargetDir@/AppRun");
		component.addElevatedOperation("Execute", 
			"desktop-file-edit", "@TargetDir@/OTPView.desktop",
			"--set-icon=@TargetDir@/OTPView.png");
		component.addElevatedOperation("Execute", 
			"desktop-file-install", "@TargetDir@/OTPView.desktop");
		component.addElevatedOperation("Execute", 
			"update-desktop-database");
	}
}