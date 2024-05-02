#!/bin/bash

BLUE='\033[1;34m'
GREEN='\033[1;32m'
RED='\033[1;31m'
NC='\033[0m' # No Color

echo -e "${BLUE}[*] Changing directory to /home/vagrant/pintos/threads${NC}"
cd /home/vagrant/pintos/threads
echo -e "${GREEN}[+] Successfully changed directory${NC}"

echo -e "${BLUE}[*] Starting build process${NC}"
make AW_DEBUG=1
if [ $? -eq 0 ]; then
echo -e "${GREEN}[+] Build process completed successfully${NC}"
else
echo -e "${RED}[-] Build process failed${NC}"
exit 1
fi

echo -e "${BLUE}[*] Changing directory to /home/vagrant/pintos/threads/build${NC}"
cd /home/vagrant/pintos/threads/build
echo -e "${GREEN}[+] Successfully changed directory${NC}"

echo -e "${BLUE}[*] Checking permissions for pintos executable: ../../utils/pintos${NC}"
if [ ! -x ../../utils/pintos ]; then
    echo -e "${BLUE}[*] Changing permission for pintos executable: ../../utils/pintos${NC}"
    chmod +x ../../utils/pintos
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}[+] Successfully changed permissions${NC}"
    else
        echo -e "${RED}[-] Failed to change permissions${NC}"
        exit 1
    fi
else
    echo -e "${GREEN}[+] Execute permissions already set${NC}"
fi

echo -e "${BLUE}[*] Starting pintos automated test: automated_warehouse 5 2A:4C:2B:2C:3A${NC}"
../../utils/pintos automated_warehouse 5 2A:4C:2B:2C:3A
if [ $? -eq 0 ]; then
echo -e "${GREEN}[+] Pintos automated test completed successfully${NC}"
else
echo -e "${RED}[-] Pintos automated test failed${NC}"
exit 1
fi

echo -e "${GREEN}[+] All tasks successfully completed${NC}"