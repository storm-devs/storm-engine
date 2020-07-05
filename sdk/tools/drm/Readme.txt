==================================================
Using the DRM tool
==================================================

1. Create a steam account just for submitting builds via DRM tool.  Set it up with a strong password. 
2. Add the account to your Steam publishing group using the steamworks user admin.
3. Give the account access to submit DRM
4. Enter the account user name and password into the drmtool.cfg file that sits with the drmtool.exe
5. run DRM tool using the appropriate path to your exe, app ID, and flags
6. If sign-in fails due to Steam Guard, see the Steam Guard section below


==================================================
Flag values
==================================================
Default: 0
Min compatibility mode: 4
Max compatibility mode: 38
                 
Try it with flag = 0, if that breaks the game, try with flag = 4, if that still breaks the game, try with flag = 38.  

==================================================
Steam Guard
==================================================
Steam Guard may block you from authenticating.  If so, do the following.

1.	Log into the partner site using the build account to force a steamguard code to be sent 
2.	Take the steam guard code from email and paste it in place of “code” in the cfg file after “auth:”
3.	Run the DRM tool
4.	Once you’ve successfully signed in and drm’ed an exe, remove the auth line from the cfg file

==================================================
Example usage
==================================================
drmtool.exe -remotedrm build_path\Auslandia.exe 107700 0

==================================================
Example drmtool.cfg file
==================================================
server:partner.steamgames.com
user:ACCOUNT_NAME
pw:VERY_STRONG_PASSWORD
universe:public
auth:STEAM_GUARD_CODE_IF_NECESSARY

==================================================
Note on applying the DRM wrapper on top of other DRM systems
==================================================
If you are applying another DRM technology to the executable, wrap the exe with Steam DRM first in max compatibility mode (38) then apply the additional DRM.  

==================================================
Note on .Net executables
==================================================
Note: The DRM tool breaks most .net executables.  If you are building a .Net executable, you can skip the DRM wrapper, but please put in your own Steam check using SteamAPI_RestartAppIfNecessary( unOwnAppID ).  
