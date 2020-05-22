FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append = " file://priv.pem"

# swupdate signing options:
SWUPDATE_SIGNING = "RSA"
SWUPDATE_PRIVATE_KEY = "${WORKDIR}/priv.pem"

python do_swuimage_append() {
    # remove the private key from list of files
    list_for_cpio.remove("priv.pem")

    # rebuild the SWU file
    line = 'for i in ' + ' '.join(list_for_cpio) + '; do echo $i;done | cpio -ov -H crc >' + os.path.join(imgdeploydir,d.getVar('IMAGE_NAME', True) + '.swu')
    os.system("cd " + s + ";" + line)

    # the original do_swuimage() function also creates a symlink (with 'ln -sf'), but as long as the file name of the
    # SWU file doesnt change it will still work
}
