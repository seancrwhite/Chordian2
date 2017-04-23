package com.chordian.droptablestudents.chordian;

/**
 * Created by seancrwhite on 4/23/17.
 */

public class ChordBuilder {
    public native String buildChord(double[] frame);

    static {
        System.loadLibrary("ChordBuilder");
    }
}
