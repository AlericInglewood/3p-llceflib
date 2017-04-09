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

  Then updated with changes from http://sldev.free.fr/libraries/sources/llceflib-20160722-src.tar.bz2
  to add linux support. Changes were manually applied making as little changes to the lindenlabs
  code as possible; changes that made no difference (re-indentation, whitespace, line wrapping,
  comments) where not applied and changes that are only needed for old (pre-C++11) compilers neither
  (ie, in-class{} initialization, nullptr, ...). I also didn't use Henri's changes regarding cache
  and cookies(cache) paths.

  After creating a repository as above, one has to sync with upstream as follows (still in branch singularity):
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

  Where `correct_autobuild_xml` is a script that I use
  to update autobuild.xml with the md5sum and url's of the latest
  packages that are currently used by singularity.

## How to update the version of CEF that LLCEFLib builds against

## Linux

I've tested this (and am using it) for linux64.
Before running autobuild you need to create a `cef_2526_LIN_64`
directory in the root of this project by extracting a tar ball
that you got from elsewhere. For example,
```bash
    wget http://sldev.free.fr/libraries/sources/cef_binary_3.2526.1371.gea9b6c8_linux64-no_tcmalloc-with_codecs.tar.bz2
    tar xjf cef_binary_3.2526.1371.gea9b6c8_linux64-no_tcmalloc-with_codecs.tar.bz2
```
