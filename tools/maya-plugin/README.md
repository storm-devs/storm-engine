# Build instructions
1. Get Maya 5.0, 8.5 or 2020. This is required because the installation contains necessary library and include files. You can probably get them somewhere else, but building was tested only on live Maya installations.
2. Set an environment variable depending on your Maya installation. Variable name is the concatenation of Maya version (Maya50, Maya85, Maya2020) and target platform (x86, x64), e.g. Maya85x64. Variable value is the path to the Maya installation (or a folder that contains include and lib folders with Maya SDK).
3. Select the appropriate configuration and target platform that match your Maya installation.
4. Build the solution.

# Usage
1. Copy the two resulting .mll files to your ``Maya installation/bin/plug-ins`` folder.
2. Copy the .mel files to ``Maya installation/scripts/others`` folder.
3. Enable the plugins from Maya.

# Acknowledgements
1. Nick Chirkov, the author of the plugin.

# Changes to original plugin
1. Adapted for newer Maya versions.
2. 0.01 scale checkbox for geometry export made functional.