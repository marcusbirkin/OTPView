function Component()
{
	if (systemInfo.kernelType === "winnt") { 
		installer.installationFinished.connect(this, Component.prototype.installVCRedist); 
		component.enabled = true;
		component.setValue("Default","true");
	} else {
		// Disabled for non Windows platforms
		component.enabled = false;
		component.setValue("Default","false");
	}
}

Component.prototype.installVCRedist = function()
{
	// Install Microsoft Visual C++ Redistributable
	console.log("Installing Microsoft Visual C++ Redistributable");
	var targetDir = installer.value("TargetDir");
	var msvc = QDesktopServices.findFiles(targetDir, "vc_redist.x??.exe");
	msvc.forEach((binary) => {
		console.log("Execute " + binary + " /quiet /norestart");
		installer.gainAdminRights();
		installer.execute(binary, "/quiet", "/norestart");
	});
}