SUMMARY = "An image derived from public hqseries-portable-image"

# Add HACH HQ Series Portable application code to Image.
IMAGE_INSTALL += " \
    r1701-apps \
"

# Assign a password to root account.
inherit extrausers
EXTRA_USERS_PARAMS += " \
    usermod -P hqd123 root; \
"
