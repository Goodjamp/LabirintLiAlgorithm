cmake_minimum_required(VERSION 3.0.0)
project(LabyrinthPassing)

set(SRC
    labyrinthBmpProcessing.c
    labyrinthBmpProcessing.h
    labyrinthProcessing.c
    labyrinthProcessing.h
    main.c
)

add_executable(${PROJECT_NAME} ${SRC})
