#创建不停机快照过程
  本过程建立backupfile备份文件并且压缩系统大小

    qemu-img snapshot -c tag1 disk
    qemu-img convert -c -f qcow2 -t none -O qcow2 -s tag1 disk backupfile
    qemu-img snapshot -d tag1 disk

#删除快照
    外置式快照删除

    virsh snapshot-list domain
    virsh snapshot-delete --domain i-447F0802 backup-timestamp --metadata

#挂载qcow2文件
    挂载本地qcow2文件

    qemu-nbd -c /dev/nbd0 file.qcow2
    mount /dev/nbd0p1 /mnt
