# Since most Github repo has moved their branch name from master to main, Default fetch by yocto is failing. Enforce the
# Branch name as 'main' and fetch using HTTPS for faster fetch. This is the just fecth patch. Post checkout, main level
# recipe will checkout custom SRCREV anyways so that main build still the same. Just a fetch patch here.
#
# URL - https://github.com/c-ares/c-ares
# TAG - https://github.com/c-ares/c-ares/releases/tag/cares-1_14_0
# Recipe: https://layers.openembedded.org/layerindex/recipe/92964/
#
SRC_URI = "\
    git://github.com/c-ares/c-ares.git;branch=main;protocol=https; \
    file://cmake-install-libcares.pc.patch \
"
