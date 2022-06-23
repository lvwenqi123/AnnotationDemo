package com.example.wb;

public class StylusMeta {
    float x;
    float y;
    float radius;
    StylusMeta() {
        x = 0;
        y = 0;
        radius = 0;
    }
    void SetValue(float fx, float fy, float fradius) {
        x = fx;
        y = fy;
        radius = fradius;
    }
    boolean Equals(StylusMeta obj) {
        return (x == obj.x) && (y == obj.y) && (radius == obj.radius);
    }
    void SetValue(StylusMeta obj) {
        SetValue(obj.x, obj.y, obj.radius);
    }
}
