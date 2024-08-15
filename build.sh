#!/bin/bash
QT_INSTALL_PATH="/opt/Qt"


echo -e "\n------ Building cmake - START ------\n"

cmake -DCMAKE_PREFIX_PATH=${QT_INSTALL_PATH}/6.7.2/gcc_64 -B build

echo -e "\n------ Building cmake - END --------\n"

