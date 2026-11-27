#pragma once
#include <cmath>
// GROWL (Nov 19 2026, Gemini's GROWL macro design, ai/gemini/answers/2026-11-19_2300-gemini-growl.md, fitted to QUAD's engine):
// one amount 0..1 -> a wavefolder before the ladder (0 .. 3.5 folds), more drive, the cutoff opened and wobbled by QUAD's own
// LFO, and after the ladder a talking vowel filter (two band-passes, U > O > A > E > I as GROWL rises, swept by the LFO).
// The sub below ~110 Hz bypasses the fold and the vowels (stays clean and mono).
struct GrowlFx {
    float ic[2][2][2] = {}, sub[2] = {}, subIn[2] = {};
    static float fold(float x) { return std::sin(1.5707963f * x); }
    float pre(int c, float x, float g, float sr) {   // before the ladder: the fold (above ~110 Hz only), mixed in by GROWL
        if (g <= 0.f) return x;
        float a = 1.f - std::exp(-2.f * 3.14159265f * 110.f / sr);
        subIn[c] += a * (x - subIn[c]);
        float hi = x - subIn[c], w = std::fmin(1.f, 1.4f * g);
        return subIn[c] + hi + (fold(hi * (1.f + 4.5f * g)) * (0.8f / (1.f + 0.6f * g)) - hi) * w;
    }
    static void bp(float* st, float x, float fc, float k, float sr, float& out) {
        float gg = std::tan(3.14159265f * std::fmin(fc, 0.45f * sr) / sr);
        float a1 = 1.f / (1.f + gg * (gg + k)), a2 = gg * a1, a3 = gg * a2;
        float v3 = x - st[1], v1 = a1 * st[0] + a2 * v3, v2 = st[1] + a2 * st[0] + a3 * v3;
        st[0] = 2.f * v1 - st[0]; st[1] = 2.f * v2 - st[1]; out = v1;
    }
    // after the ladder: the vowel filter (lfo -1..1 = QUAD's cutoff LFO)
    float post(int c, float x, float g, float lfo, float sr) {
        if (g <= 0.f) return x;
        static const float F1[5] = {300.f, 500.f, 730.f, 530.f, 270.f}, F2[5] = {870.f, 950.f, 1090.f, 1840.f, 2290.f};   // U O A E I
        float a = 1.f - std::exp(-2.f * 3.14159265f * 110.f / sr);
        sub[c] += a * (x - sub[c]);
        float hi = x - sub[c];
        float v = std::fmin(1.f, std::fmax(0.f, 0.15f + 0.6f * g + 0.4f * g * lfo)) * 4.f;
        int i = (int) v; if (i > 3) i = 3; float f = v - (float) i;
        float f1 = F1[i] + (F1[i + 1] - F1[i]) * f, f2 = F2[i] + (F2[i + 1] - F2[i]) * f;
        float b1, b2;
        bp(ic[c][0], hi, f1, 0.18f, sr, b1);
        bp(ic[c][1], hi, f2, 0.16f, sr, b2);
        float voc = (b1 + 0.75f * b2) * 0.9f;
        float wet = std::fmin(1.f, 1.2f * g);
        return sub[c] + (hi * (1.f - 0.75f * wet) + voc * wet) / (1.f + 0.5f * g);   // (trimmed: GROWL 1 stays under the clip)
    }
};
