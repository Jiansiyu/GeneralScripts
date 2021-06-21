ldd `which analyzer` | fgrep libHallA|awk '{print $3}'|xargs dirname
