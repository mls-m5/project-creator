#!/usr/bin/bash

DEST_DIR="/usr/bin"

function create_symlink {
    if [ ! -e "$1" ]; then
        echo "Warning: Local file '$1' does not exist. Skipping..."
        return 1
    fi

    src=$(realpath "$1")

    dest="$DEST_DIR/$(basename "$1")"

    if [ -e "$dest" ]; then
        sudo rm "$dest"
        echo "Removed existing file: $dest"
    fi

    sudo ln -s "$src" "$dest"
    echo "Created symlink: $dest -> $src"
}

create_symlink "build"
create_symlink "create-project"
create_symlink "mfind"
create_symlink "project-goto"
create_symlink "mopen"
create_symlink "mgit"
