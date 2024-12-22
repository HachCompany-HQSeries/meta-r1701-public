# Add branch as main since most of the Github repos have moved from Master to Main, this BBAPPEND to resolve the master
# branch fetch error, solve it by forcing the branch name as 'main' for checkout. After checkout custom SRCREV will be 
# checked out anyways so we're good from post checkout and build standpoint of view. SRCREV is defined at main BB level.
#
# URL: https://github.com/google/googletest
# TAg: https://github.com/google/googletest/releases/tag/release-1.8.0
# Recipe: https://layers.openembedded.org/layerindex/recipe/91932/
#
SRC_URI = "\
    git://github.com/google/googletest.git;branch=main;protocol=https; \
    file://Add-pkg-config-support.patch \
"
