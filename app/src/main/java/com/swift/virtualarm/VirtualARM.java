package com.swift.virtualarm;

public class VirtualARM {

    static {
        System.loadLibrary("virtual_arm");
    }

    public static native void launch();
}
