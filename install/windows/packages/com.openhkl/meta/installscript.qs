function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    // Call the default implementation to install all files in the package
    component.createOperations();

    // Create a Start Menu shortcut for the application
    component.addOperation("CreateShortcut", "@TargetDir@/OpenHKL.exe", "@StartMenuDir@/OpenHKL.lnk",
                           "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/icons/OpenHKL.ico",
                           "description=Launch OpenHKL Application");

    // Create a Start Menu shortcut for the Maintenance Tool
    component.addOperation("CreateShortcut", "@TargetDir@/maintenancetool.exe", "@StartMenuDir@/Uninstall\ OpenHKL.lnk",
                           "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/icons/OpenHKL.ico",
                           "description=Open OpenHKL Maintenance Tool");
};
