#!/bin/bash

cd src/

if [[ "$OSTYPE" == "linux-gnu" ]]; then
    gcc -o ../osu *.c -lglew -lgl -lglfw -lm
elif [[ "$OSTYPE" == "darwin"* ]]; then
    gcc -o ../osu *.c -lglew -framework OpenGL -lglfw -lm
else
    echo "OS currently not supported"
fi
cd ..

echo "Done!"