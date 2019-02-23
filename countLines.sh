#!/bin/bash

find . -name '*.h' -or -name '*.cpp' | xargs wc -l
