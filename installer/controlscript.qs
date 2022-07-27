function Controller()
{
	if (installer.isInstaller()) {
	}

	if (installer.isUninstaller()) {
		// Create simple uninstall only uninstaller
		installer.setDefaultPageVisible(QInstaller.Introduction, false);
		installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
		installer.setDefaultPageVisible(QInstaller.LicenseCheck, false);
	}
}
