# GDB bug prevents referencing "keys:" label in main()

dprintf main.c:135, \
"%c%c%c%c\n%c%c%c%c\n%c%c%c%c\n\n", \
(keys_mapd & 0x800) ? '*' : '.', \
(keys_mapd & 0x400) ? '*' : '.', \
(keys_mapd & 0x200) ? '*' : '.', \
(keys_mapd & 0x100) ? '*' : '.', \
(keys_mapd & 0x080) ? '*' : '.', \
(keys_mapd & 0x040) ? '*' : '.', \
(keys_mapd & 0x020) ? '*' : '.', \
(keys_mapd & 0x010) ? '*' : '.', \
(keys_mapd & 0x008) ? '*' : '.', \
(keys_mapd & 0x004) ? '*' : '.', \
(keys_mapd & 0x002) ? '*' : '.', \
(keys_mapd & 0x001) ? '*' : '.'

disable
