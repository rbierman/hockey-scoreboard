#!/bin/bash
# PuckPulse Controller Auto-Update Script

REPO="rbierman/puckpulse"
PACKAGE="puckpulse-controller"

# Get currently installed version
INSTALLED_VER=$(dpkg-query -W -f='${Version}' $PACKAGE 2>/dev/null)

# Fetch latest release info from GitHub
LATEST_RELEASE_JSON=$(curl -s https://api.github.com/repos/$REPO/releases/latest)
LATEST_VER=$(echo "$LATEST_RELEASE_JSON" | jq -r .tag_name | sed 's/v//')

if [ -z "$LATEST_VER" ] || [ "$LATEST_VER" == "null" ]; then
    echo "Error: Could not fetch latest version from GitHub."
    exit 1
fi

if [ "$INSTALLED_VER" != "$LATEST_VER" ]; then
    echo "Update available: $INSTALLED_VER -> $LATEST_VER"
    
    # Detect if we are currently running headless or standard
    # Standard build depends on libX11, headless does not.
    if ldd /usr/bin/puckpulse-controller | grep -q "libX11"; then
        echo "Detected Standard installation."
        ASSET_PATTERN="puckpulse-controller-.*-Linux.deb"
        # Specifically EXCLUDE headless
        DEB_URL=$(echo "$LATEST_RELEASE_JSON" | jq -r '.assets[] | select(.name | test("headless") | not) | select(.name | endswith(".deb")) | .browser_download_url' | head -n 1)
    else
        echo "Detected Headless installation."
        DEB_URL=$(echo "$LATEST_RELEASE_JSON" | jq -r '.assets[] | select(.name | test("headless")) | select(.name | endswith(".deb")) | .browser_download_url' | head -n 1)
    fi
    
    if [ -z "$DEB_URL" ] || [ "$DEB_URL" == "null" ]; then
        echo "Error: Could not find .deb asset in the latest release."
        exit 1
    fi

    echo "Downloading $DEB_URL..."
    TMP_DEB="/tmp/puckpulse_update.deb"
    curl -L -o "$TMP_DEB" "$DEB_URL"
    
    echo "Installing update..."
    # Using DEBIAN_FRONTEND=noninteractive to ensure it doesn't hang on prompts
    DEBIAN_FRONTEND=noninteractive apt-get install -y "$TMP_DEB"
    
    rm "$TMP_DEB"
    echo "Update complete."
else
    echo "PuckPulse is up to date ($INSTALLED_VER)."
fi
