#!/usr/bin/env bash
# -*- mode: shell-script; indent-tabs-mode: nil; sh-basic-offset: 4; -*-
# ex: ts=8 sw=4 sts=4 et filetype=sh
set -eux
set -o pipefail

export SYSTEMD_LOG_LEVEL=debug

portablectl attach --now --runtime /usr/share/minimal_0.raw app0

systemctl is-active app0.service
systemctl is-active app0-foo.service
set +o pipefail
set +e
systemctl is-active app0-bar.service && exit 1
set -e
set -o pipefail

portablectl reattach --now --runtime /usr/share/minimal_1.raw app0

systemctl is-active app0.service
systemctl is-active app0-bar.service
set +o pipefail
set +e
systemctl is-active app0-foo.service && exit 1
set -e
set -o pipefail

portablectl list | grep -q -F "minimal_1"

portablectl detach --now --runtime /usr/share/minimal_1.raw app0

portablectl list | grep -q -F "No images."

# portablectl also works with directory paths rather than images

unsquashfs -dest /tmp/minimal_0 /usr/share/minimal_0.raw
unsquashfs -dest /tmp/minimal_1 /usr/share/minimal_1.raw

portablectl attach --copy=symlink --now --runtime /tmp/minimal_0 app0

systemctl is-active app0.service
systemctl is-active app0-foo.service
set +o pipefail
set +e
systemctl is-active app0-bar.service && exit 1
set -e
set -o pipefail

portablectl reattach --now --enable --runtime /tmp/minimal_1 app0

systemctl is-active app0.service
systemctl is-active app0-bar.service
set +o pipefail
set +e
systemctl is-active app0-foo.service && exit 1
set -e
set -o pipefail

portablectl list | grep -q -F "minimal_1"

portablectl detach --now --enable --runtime /tmp/minimal_1 app0

portablectl list | grep -q -F "No images."

root="/usr/share/minimal_0.raw"
app1="/usr/share/app1.raw"

portablectl attach --now --runtime --extension ${app1} ${root} app1

systemctl is-active app1.service

portablectl reattach --now --runtime --extension ${app1} ${root} app1

systemctl is-active app1.service

portablectl detach --now --runtime --extension ${app1} ${root} app1

# portablectl also works with directory paths rather than images

mkdir /tmp/rootdir /tmp/app1 /tmp/overlay
mount ${app1} /tmp/app1
mount ${root} /tmp/rootdir
mount -t overlay overlay -o lowerdir=/tmp/app1:/tmp/rootdir /tmp/overlay

portablectl attach --copy=symlink --now --runtime /tmp/overlay app1

systemctl is-active app1.service

portablectl detach --now --runtime overlay app1

umount /tmp/overlay
umount /tmp/rootdir
umount /tmp/app1

echo OK >/testok

exit 0
