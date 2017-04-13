# LLCEFLib #

## Purpose

LLCEFLib uses the [Chromium Embedded Framework](https://en.wikipedia.org/wiki/Chromium_Embedded_Framework) (CEF) to render web content to a memory buffer that is then used in the [Second Life viewer](http://secondlife.com) to display media on 2D and 3D objects.

## Origin

This is basically a fork of [LindenLabs llceflib](https://bitbucket.org/lindenlab/3p-llceflib).

* Created as follows:

```bash
    git-hg clone https://bitbucket.org/lindenlab/3p-llceflib 3p-llceflib
    cd 3p-llceflib
    git checkout -b singularity
```

  Then updated with changes as [published by CoolViewer](http://sldev.free.fr/libraries/sources/llceflib-20160722-src.tar.bz2)
  by Henri Beauchamp to add linux support. Changes were manually applied making as little changes to the lindenlabs
  code as possible; changes that made no difference (re-indentation, whitespace, line wrapping,
  comments) where not applied and changes that are only needed for old (pre-C++11) compilers neither
  (ie, in-class{} initialization, nullptr, ...). I also didn't use Henri's changes regarding cache
  and cookies(cache) paths. The changes by Henri are marked with &lt;CV:HB&gt;, except where it
  concerns specific linux code where his authorship is implied.
  
  Finally, I manually looked over all commits in [alchemy's repository](https://bitbucket.org/alchemyviewer/3p-alceflib)
  that doesn't seem to be based on anything (they didn't make a clone, but just added files to
  a "new" repository) and seems to include a lot of changes from everywhere (most notably LL,
  Henry and NickyD) without doing much creditting; everything is applied manually causing the
  indentation to differ (Drake uses TABs while there are no TABs in LL's repository); hence I had
  to do these patches by hand too. Because I had no idea who really authored what, I marked them
  with &lt;SV:AI&gt;. Among others this means we're now using (a re-indented) version of NickyD's
  `llceflibimpl_linux.cpp` (like FireStorm, Alchemy (== Singularity's original)).

  After creating a repository as above (with git-hg), one has to sync with upstream (Linden Lab)
  as follows (still in branch singularity):
```bash
    git-hg fetch
    git merge master
```

* To build the package (in branch singularity):

```bash
    correct_autobuild_xml
    git diff
    git commit -a -m 'Updated urls of prebuilt packages.'
    autobuild install
    autobuild build
    autobuild package
```

  Where `correct_autobuild_xml` is a script that I use to update autobuild.xml with the md5sum and
  urls of the latest packages that are currently used by singularity.

## How to update the version of CEF that LLCEFLib builds against

## Linux x86_64

I've tested this (and am using it) for linux64.
Before running autobuild you need to build CEF3; for that you need at least 6 GB of RAM
and 40 GB of free disk space.

The following is a blend of [BranchesAndBuilding](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding)
and [MasterBuildQuickStart](https://bitbucket.org/chromiumembedded/cef/wiki/MasterBuildQuickStart.md)
from the cef wiki, improved, fixed and tuned to work with this repository.

1. Set environment.

  ```bash
  export BASE=/path/to/root/of/3p-llceflib
  export BRANCH=3029
  export AUTOBUILD_PLATFORM_OVERRIDE=linux64
  export CHROME_DEVEL_SANDBOX=/usr/local/sbin/chrome-devel-sandbox
  export PATH="$BASE/depot_tools:$PATH"
  ```

  here `$BASE` should be the directory containing `build-cmd.sh` and `.git`
  and may not contain spaces.

  `$BRANCH` should be a recent enough branch. See the table of [Current Release Branches](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding#markdown-header-current-release-branches-supported).
  The branch must be at least 2785 for these instructions to work; if the branch is less than 2840 you must als set `export CEF_USE_GN=1`.

2. Download and run `"~/code/install-build-deps.sh"` to install build dependencies. Answer Y (yes) to all of the questions.

  ```bash
  cd $BASE
  curl 'https://chromium.googlesource.com/chromium/src/+/master/build/install-build-deps.sh?format=TEXT' | base64 -d > install-build-deps.sh
  chmod 755 install-build-deps.sh
  sudo ./install-build-deps.sh --no-syms --no-arm --no-chromeos-fonts --no-nacl
  ```

3. Install the `"libgtkglext1-dev"` package required by the cefclient sample application.

  ```bash
  sudo apt install libgtkglext1-dev
  ```

4. Download the `depot_tools` using git.

  ```bash
  cd $BASE
  git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
  ```

5. Download Chromium source code using the fetch tool included with `depot_tools`.
   This step only needs to be performed the first time Chromium code is checked out.
   *This will download > 7GB of git repository*.

  ```bash
  mkdir -p $BASE/chromium_git/chromium
  cd $BASE/chromium_git/chromium
  fetch --nohooks chromium
  ```

6. Download additional branch and tag information.

  ```bash
  cd $BASE/chromium_git/chromium/src
  gclient sync --nohooks --with_branch_heads
  git fetch
  git fetch --tags
  ```

7. Download CEF source code from the CEF Git repository to a "cef" directory inside the Chromium "src" directory.

  ```bash
  cd $BASE/chromium_git/chromium/src
  git clone https://bitbucket.org/chromiumembedded/cef.git
  cd cef
  git checkout -t origin/$BRANCH
  ```

8. Check out the correct chromium release version and update the third-party dependencies.

  ```bash
  cd $BASE/chromium_git/chromium/src
  git checkout $(sed -n -e "s/.*'\(refs[^']*\)'.*/\1/p" cef/CHROMIUM_BUILD_COMPATIBILITY.txt)
  gclient sync --jobs 16
  ```

  Here the `sed` command extracts the refspec from `cef/CHROMIUM_BUILD_COMPATIBILITY.txt`
  which should look something like `refs/tags/58.0.3029.33`.

9. Create an executable `"$BASE/chromium_git/chromium/src/cef/create.sh"` script by executing the following on the command line:

  ```base
  cd $BASE/chromium_git/chromium/src/cef
  cat > create.sh << EOF
  #!/bin/bash
  export CEF_USE_GN=1
  ./cef_create_projects.sh
  EOF
  chmod 755 create.sh
  ```

10. Run the `"create.sh"` script to create Ninja project files. Repeat this step if you change the project configuration or add/remove files in the GN configuration (BUILD.gn file).

  ```bash
  cd $BASE/chromium_git/chromium/src/cef
  ./create.sh
  ```

11. Set up the Linux SUID sandbox. These command only needs to be run a single time. It uses sudo which might ask for your password.

  ```bash
  cd $BASE/chromium_git/chromium/src
  sudo rm -f "$CHROME_DEVEL_SANDBOX"
  ninja -C out/Release_GN_x64 chrome_sandbox
  BUILDTYPE=Release_GN_x64 ./build/update-linux-sandbox.sh
  ```

12. Create a Debug build of CEF/Chromium using Ninja. Edit the CEF source code at `"$BASE/chromium_git/chromium/src/cef"`
    and repeat this step multiple times to perform incremental builds while developing.

  ```bash
  cd $BASE/chromium_git/chromium/src
  ninja -C out/Debug_GN_x64 cef
  ```

13. To test if this worked, run the cefclient sample application.

  ```bash
  cd $BASE/chromium_git/chromium/src
  ./out/Debug_GN_x64/cefclient
  ```

14. Create a Release build of CEF/Chromium.

  ```bash
  cd $BASE/chromium_git/chromium/src
  ninja -C out/Release_GN_x64 cef
  ```

15. Create the binary package for CEF.

  ```bash
  cd $BASE/chromium_git/chromium/src/cef/tools
  ./make_distrib.sh --ninja-build --x64-build --no-docs --distrib-subdir=cef_"$BRANCH"_LIN_64
  ```

16. Unpack binary distribution in root of the project.

  ```bash
  cd $BASE
  rm -rf cef_"$BRANCH"_LIN_64
  unzip $BASE/chromium_git/chromium/src/cef/binary_distrib/cef_"$BRANCH"_LIN_64.zip
  ```
