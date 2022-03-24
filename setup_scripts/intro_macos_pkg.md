The NSXTool zip-package for MacOS includes all the required dependencies (excluding Qt) can be downloaded as a GitLab artifact according to the CPU architecture (x86_64 or arm64).

* Usage:
1) Install Qt5 *once*: If Qt5 is _not_ installed on the system, open a shell and execute either of the following commands:

1a) With Homebrew:
```
brew install qt@5
```

If you do not know if Qt5 was installed, use the following:
```
brew list | grep -iF qt
```
You should see `qt@5` in the result.

1b) With MacPorts
```
port install qt5
```

If you do not know if Qt5 was installed, use the following:
```
port installed | grep -iF qt
```

Note that step 1) does NOT need be repeated for later installations.

2) Download and uncompress the zip-package in a preferred folder:
2a) Download the zip-package from one of the links above, depending on the CPU architecture. The command `uname -m` can be used to obtain the CPU architecture.
2b) Uncompress:
```
unzip nsxtool_macos.zip
```

3) Run NSXTool:
```
cd nsx
./NSXTool
```

* Problems:

1) Security warnings:
NSXTool has currently no MacOS certificates, therefore MacOS might warn against executing NSXTool.

In case of the MacOS security popups (from the 'Gatekeeper'), enter the following command in a terminal
```
sudo spctl --master-disable
```
and then, go to System Preferences > Security & Privacy > General. Click on the lock icon (bottom left) to unlock the configuration. Under "Allow apps downloaded from:" header, check "Anywhere".

Finally, the command
```
sudo spctl --master-enable
```
will return the macOS Gatekeeper back to its strict default state.

2) NSXTool executable:
If the executable does not run properly, then run the included debugging script:
```
bash dbg_macos_pack.sh nsx.log
```
to gather detailed information about the machine and its configuration.
Please send the produced log files, `nsx.log` and `testexe.log`, to the NSXTool developers via e-mail.
