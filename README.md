##### ara-fw-test-public

1. Download NDK  
   `http://dl.google.com/android/ndk/android-ndk-r10e-linux-x86_64.bin`
2. Make it executable  
   `chmod a+x android-ndk-r10e-linux-x86_64.bin`
3. Install NDK  
   `./android-ndk-r10e-linux-x86_64.bin`
4. Extract toolchain  
   `sudo /home/<USER>/android-ndk-r10e/build/tools/make-standalone-toolchain.sh \`  
   `  --toolchain=aarch64-linux-android-4.9 --system=linux-x86_64 --platform=android-21 \ `   
   `  --install-dir=/opt/android-toolchain-aarch64-4.9-host-linux-x86_64-android-21 `  
5. Make toolchain runnable  
   `cd /opt`  
   `sudo chmod -R +x android-toolchain-aarch64-4.9-host-linux-x86_64-android-21`  
6. Clone and build this repository  
   `cd ~`  
   `git clone https://github.com/darryln/ara-fw-test-public.git`  
   `cd ara-fw-test-public`  
   `make all`  
7. You can also `make --always-make` to force rebuild, and `make clean`.  
8. Test app executables are placed under `~/ara-fw-test-public/build/apps/`.  

##### Notes
* By default, all functional test apps dump their command line args by calling dumpargs, which is part of the common test app library, libfwtest.a.

* To add code to libfwtest.a, put your .c file in apps/lib and declare the functions you want to expose in apps/lib/include/libfwtest.h.  All .c files under apps/lib get built into libfwtest.a automatically, so there is no need to update the Makefile for it.

* New test apps can be created under apps/functional, apps/greybus, apps/stress, apps/performance, and apps/other
  * mkdir \<name of test app\>
  * copy an existing test app and Makefile there
  * modify Makefile and source files as needed.
  * There is no need to modify higher-level Makefiles, the newly added test app will get built automatically.
  * If you add a test, you must also update the supported test cases in TestLink. Each automated test case must have a test app. 

Commit messages should be formatted as follows:
```
         1         2         3         4         5         6         7
1234567890123456789012345678901234567890123456789012345678901234567890123456
```
```
[name of test app]:  One-line short message, max 60 columns. 
<blank line>
This is the long message. Please be descriptive and provide as much detail 
as possible. This message should provide enough info to allow someone to 
complete the statement "If you apply this patch, it will ________."  
Paragraphs in the long message should be manually wrapped at 76 columns. 
<blank line>
Test case: [list all test cases supported or impacted by this commit]
Testing done:[brief one-line description only, use the RB "Testing Done" 
 section to capture more details]
Bug number: [JIRA defect assigned, otherwise leave blank]
Review URL:  http://externalgit2.bsquare.com/reviewboard/r/[review number]/
Reviewed by:  Bob Hacker <hacker_bob@projectara.com>, 
  Jenny Jira <jira_jenny@projectara.com>
Signed-off-by:  Joe Coder <coder_joe@projectara.com> 
```
Note: if you have set up your .gitconfig with your name and email,  'git -s' adds the "Signed-off-by: your name <your email>" automatically.  However, best practice is to use a .gitmessage template in your .gitconfig:  
`[commit]`  
    `template = ~/.gitmessage`

Note: the test app Makefile (i.e. the Makefile contained in the same folder as the app's source code) generates the app executable with the same name as the containing folder.

i.e.
apps/greybus/i2ctest   -> build/apps/i2ctest
apps/functional/cam_test -> build/apps/cam_test
apps/other/sometest -> build/apps/sometest

This is the desired behavior based on the APP variable defined in the Makefile, do not modify.

