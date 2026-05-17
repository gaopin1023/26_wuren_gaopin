#!/bin/bash
# 创建目录
mkdir -p linux_practice/docs linux_practice/backup

# 创建3个文件
touch linux_practice/docs/readme.txt
touch linux_practice/docs/notes.log
touch linux_practice/docs/temp.tmp

# 删除文件
rm linux_practice/docs/temp.tmp

# 重命名文件
mv linux_practice/docs/notes.log linux_practice/docs/daily_report.txt

# 写入内容
echo "Project Status: Active" > linux_practice/docs/daily_report.txt
date >> linux_practice/docs/daily_report.txt

# 复制所有txt到backup
cp linux_practice/docs/*.txt linux_practice/backup/

# 设为只读权限
chmod 444 linux_practice/backup/*

echo "Archive Complete. File readme.txt is now read-only."
echo "Archive Complete. File daily_report.txt is now read-only."
