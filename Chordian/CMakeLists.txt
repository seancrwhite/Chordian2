cmake_minimum_required(VERSION 3.4.1)

add_library( ChordBuilder,
             SHARED,
             ../ExternalLib/src/ChordBuilder.cpp )

include_directories( ../ExternalLib/src )