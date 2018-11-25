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

## How to install/update the version of CEF that LLCEFLib builds against

## Linux x86_64

I've tested this (and am using it) for linux64.

1. Set environment.

  ```bash
  export REPOBASE=/path/to/root/of/3p-llceflib
  export AUTOBUILD_PLATFORM_OVERRIDE=linux64
  ```

  here `$REPOBASE` should be the directory containing `build-cmd.sh` and `.git`
  and may not contain spaces.

2. Download binary CEF distribution.

  ```bash
  cd $REPOBASE
  wget http://opensource.spotify.com/cefbuilds/cef_binary_3.3538.1852.gcb937fc_linux64.tar.bz2
  tar xf cef_binary_3.3538.1852.gcb937fc_linux64.tar.bz2
  ```

  Of course you can use the latest version; but that might require
  some patches to take API changes into account.

3. Fix build-cmd.sh

  Edit build-cmd.sh and set `CEF_VERSION` to the correct value (see step 2).

  Now the source tree is ready for autobuild.
