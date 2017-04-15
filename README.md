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

The following is a blend of [AutomatedBuildSetup](https://bitbucket.org/chromiumembedded/cef/wiki/AutomatedBuildSetup),
[BranchesAndBuilding](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding)
and [MasterBuildQuickStart](https://bitbucket.org/chromiumembedded/cef/wiki/MasterBuildQuickStart.md)
from the cef wiki, improved, fixed and tuned to work with this repository.

1. Set environment.

  ```bash
  export REPOBASE=/path/to/root/of/3p-llceflib
  export BASE=$REPOBASE/chromium_git
  export BRANCH=3029
  export AUTOBUILD_PLATFORM_OVERRIDE=linux64
  export CHROME_DEVEL_SANDBOX=/usr/local/sbin/chrome-devel-sandbox
  export PATH="$BASE/depot_tools:$PATH"
  export CEF_USE_GN=1
  export GN_DEFINES="is_official_build=true use_sysroot=true use_allocator=none symbol_level=1"
  export GYP_DEFINES="disable_nacl=1 use_sysroot=1 buildtype=Official use_allocator=none"
  export CEF_ARCHIVE_FORMAT=tar.bz2
  ```

  here `$REPOBASE` should be the directory containing `build-cmd.sh` and `.git`
  and may not contain spaces.

  `$BRANCH` should be a recent enough branch. See the table of [Current Release Branches](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding#markdown-header-current-release-branches-supported).
  The branch must be at least 2785 for these instructions to work; if the branch is less than 2840 you must als set `export CEF_USE_GN=1`.

2. Run `install-build-deps.sh` to install build dependencies. Answer Y (yes) to all of the questions if any. This has to be done only once.

  ```bash
  cd $BASE
  sudo ./install-build-deps.sh --no-syms --no-arm --no-chromeos-fonts --no-nacl
  ```

  If you don't trust this script of want the latest version then feel free to recreate it with

  ```bash
  curl 'https://chromium.googlesource.com/chromium/src/+/master/build/install-build-deps.sh?format=TEXT' | base64 -d > install-build-deps.sh
  chmod 755 install-build-deps.sh
  ```

3. Install the `"libgtkglext1-dev"` package required by the cefclient sample application. This has to be done only once.

  ```bash
  sudo apt install libgtkglext1-dev
  ```

4. Download the `depot_tools` and `chromium_git/cef` git submodules (they are already checked out if you cloned this repository using `git clone --recursive ...`).
   Note that later on cef will be copied to `chromium_git/chromium/src/cef` and possibly changed there. We need a copy here though so the scripts
   can make a "fresh" copy again later when needed...

  ```bash
  cd $BASE
  git submodule update
  ```

5. Download Chromium source code using the fetch tool included with the `depot_tools`.
   This step only needs to be performed the first time Chromium code is checked out.
   *This will download > 7GB of git repository*. This is not the only reason why I
   didn't make this a git submodule; the cef scripts will patch the checkout with
   a series of diff's after which things like 'git update' would fail due to merge
   problems. It's best to use the `depot_tools`, run by cef provided scripts to
   deal with this git repository. $BASE/chromium was even added to our `.gitignore`.

  ```bash
  python automate/automate-git.py --download-dir=$BASE --branch=$BRANCH --x64-build --force-update --no-build --no-distrib
  ```

6. Create a Debug and Release builds of CEF/Chromium.

  ```bash
  python automate/automate-git.py --download-dir=$BASE --branch=$BRANCH --x64-build --force-build --build-target=cefsimple
  ```

  To build only a Debug build or Release build, add `--no-release-build` or `--no-debug-build` respectively,
  and `--no-distrib` (you can't make a package unless you have both built).

  If this is the first time you run this command and you just ran the command at point 5.
  then you might as well replace the `--force-build` with `--no-update`.

7. Unpack binary distribution in root of the project.

  ```bash
  cd $BASE
  tar xjf chromium_git/chromium/src/cef/binary_distrib/cef_binary_3.$BRANCH.*_linux64.tar.bz2
  ```

  Now the source tree is ready for autobuild.
