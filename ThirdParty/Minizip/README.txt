The sources for minizip are copied from: https://github.com/madler/zlib/releases/tag/v1.3.1

Additionally, a few changes have been made:
1. Added the miniunz.h/minizip.h headers
2. The main method in minizip.c has been renamed to minizip.
3. The main method in miniunz.c as been renamed to miniunz.
4. Use a dummy minizip_printf instead of printf
