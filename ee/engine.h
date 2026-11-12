#pragma once
// ---------------------------------------------------------------------------
// ee/engine.h - OPTIONAL easter egg engine pieces: procedural audio, shaders and the renderer.
// ---------------------------------------------------------------------------
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include "ui/Viewport.h"

namespace sh2 {

constexpr float kPi = 3.14159265f;

struct Rng {
    uint32_t s = 2463534242u;
    float F() { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return (s & 0xFFFFFF) / 16777216.0f; }
    float S() { return F() * 2.0f - 1.0f; }
};

using Buf = std::vector<float>;
constexpr int kRate = 22050;

inline Sound ToSound(const Buf& b, float gain = 1.0f) {
    Wave w{}; w.frameCount = (unsigned)b.size(); w.sampleRate = kRate; w.sampleSize = 16; w.channels = 1;
    short* d = (short*)std::malloc(b.size() * sizeof(short));
    for (size_t i = 0; i < b.size(); i++) d[i] = (short)(std::max(-1.0f, std::min(1.0f, b[i] * gain)) * 32000.0f);
    w.data = d;
    Sound s = LoadSoundFromWave(w);
    UnloadWave(w);
    return s;
}
inline void LoopFade(Buf& g, size_t n, size_t fade) {
    for (size_t i = 0; i < fade; i++) { float w = (float)i / fade; g[i] = g[i] * w + g[n + i] * (1 - w); }
    g.resize(n);
}
inline float Env(float t, float k) { return std::exp(-t * k); }
inline float Tone(float f, float t) { return std::sin(2 * kPi * f * t); }

inline Buf SynDrone() {
    Rng r; size_t n = kRate * 8, f = kRate; Buf g(n + f); float lp = 0, lp2 = 0;
    for (size_t i = 0; i < g.size(); i++) {
        float t = (float)i / kRate;
        lp += 0.015f * (r.S() - lp); lp2 += 0.004f * (r.S() - lp2);
        float v = 0.34f * Tone(55, t) * (0.75f + 0.25f * Tone(0.25f, t))
                + 0.20f * std::sin(2 * kPi * 82.5f * t + 0.8f * Tone(0.125f, t))
                + 0.08f * Tone(110, t) + 1.2f * lp * (0.6f + 0.4f * Tone(0.25f, t + 0.2f)) + 2.4f * lp2;
        g[i] = v * 0.7f;
    }
    LoopFade(g, n, f); return g;
}
inline Buf SynWind() {
    Rng r; size_t n = kRate * 6, f = kRate; Buf g(n + f); float lp = 0, lp2 = 0;
    for (size_t i = 0; i < g.size(); i++) {
        float t = (float)i / kRate;
        lp += 0.05f * (r.S() - lp); lp2 += 0.012f * (r.S() - lp2);
        g[i] = (lp * 1.5f + lp2 * 3.0f) * (0.55f + 0.45f * Tone(0.5f, t)) * 0.8f;
    }
    LoopFade(g, n, f); return g;
}
inline Buf SynStatic() {
    Rng r; size_t n = kRate * 2, f = kRate / 2; Buf g(n + f); float env = 0;
    for (size_t i = 0; i < g.size(); i++) {
        if (r.F() > 0.9992f) env = 0.5f + 0.5f * r.F();
        env *= 0.9992f;
        g[i] = r.S() * (0.20f + env * 0.5f) * 0.65f;
    }
    LoopFade(g, n, f); return g;
}
inline Buf SynStep(int v) {
    Rng r; r.s += v * 977u; size_t n = kRate * 16 / 100; Buf g(n); float lp = 0;
    for (size_t i = 0; i < n; i++) {
        float t = (float)i / kRate; lp += 0.18f * (r.S() - lp);
        g[i] = lp * Env(t, 28) * 2.4f + 0.5f * Tone(48.0f + v * 9, t) * Env(t, 22);
    }
    return g;
}
inline Buf SynTick() {
    size_t n = kRate * 7 / 100; Buf g(n);
    for (size_t i = 0; i < n; i++) { float t = (float)i / kRate; g[i] = Tone(740, t) * Env(t, 55) * 0.5f + Tone(1480, t) * Env(t, 70) * 0.15f; }
    return g;
}
inline Buf SynOpen() {
    Rng r; size_t n = kRate * 55 / 100; Buf g(n); float lp = 0;
    for (size_t i = 0; i < n; i++) {
        float t = (float)i / kRate, p = t / 0.55f, e = std::sin(kPi * p); e *= e;
        lp += (0.02f + 0.3f * p) * (r.S() - lp);
        g[i] = (lp * 1.8f + 0.35f * Tone(85, t)) * e;
    }
    return g;
}
inline Buf SynSelect() {
    size_t n = kRate * 18 / 100; Buf g(n);
    for (size_t i = 0; i < n; i++) { float t = (float)i / kRate; g[i] = (Tone(330, t) + 0.6f * Tone(495, t)) * Env(t, 18) * 0.4f; }
    return g;
}
inline Buf SynHit() {
    Rng r; size_t n = kRate * 35 / 100; Buf g(n); float lp = 0;
    for (size_t i = 0; i < n; i++) { float t = (float)i / kRate; lp += 0.25f * (r.S() - lp); g[i] = Tone(62, t) * Env(t, 12) * 0.55f + lp * Env(t, 22) * 0.8f; }
    return g;
}
inline Buf SynHeart() {
    size_t n = kRate * 11 / 10; Buf g(n);
    for (size_t i = 0; i < n; i++) {
        float t = (float)i / kRate, v = 0;
        if (t > 0) v += Tone(52, t) * Env(t, 16);
        if (t > 0.3f) v += 0.7f * Tone(48, t - 0.3f) * Env(t - 0.3f, 16);
        g[i] = v * 0.8f;
    }
    return g;
}
inline Buf SynStinger() {
    Rng r; size_t n = kRate * 5; Buf g(n); float lp = 0;
    static const float fq[4] = {523.3f, 709.6f, 1187.4f, 1613.1f}, am[4] = {0.22f, 0.16f, 0.10f, 0.07f};
    for (size_t i = 0; i < n; i++) {
        float t = (float)i / kRate;
        float swell = std::min(t / 1.5f, 1.0f) * Env(std::max(0.0f, t - 2.0f), 0.6f);
        lp += 0.03f * (r.S() - lp);
        float v = 0.5f * Tone(41, t) * swell + lp * 2.0f * swell;
        for (int k = 0; k < 4; k++) v += am[k] * Tone(fq[k], t) * Env(t, 1.1f);
        float fade = std::min(1.0f, (5.0f - t) / 0.5f);
        g[i] = v * 0.8f * fade;
    }
    return g;
}
inline Buf SynModem() {
    Rng r; size_t n = kRate * 32 / 10; Buf g(n);
    for (size_t i = 0; i < n; i++) {
        float t = (float)i / kRate, v;
        if (t < 0.4f) v = Tone(2100, t);
        else if (t < 0.8f) v = Tone(1300, t);
        else if (t < 1.6f) v = Tone(((int)(t * 20) % 2) ? 2400.0f : 1200.0f, t);
        else v = (r.F() > 0.5f ? 1.0f : -1.0f) * 0.5f * (((int)(t * 8) % 3) ? 1.0f : 0.2f) + 0.4f * Tone(1800, t);
        g[i] = v * 0.22f * std::min(1.0f, (3.2f - t) / 0.3f);
    }
    return g;
}

class Audio {
public:
    ~Audio() { Shutdown(); }
    bool Ready() const { return ready; }
    void Init() {
        if (!IsAudioDeviceReady()) { InitAudioDevice(); own = true; }
        ready = IsAudioDeviceReady();
        if (!ready) return;
        Add("drone", SynDrone()); Add("wind", SynWind()); Add("static", SynStatic());
        Add("step0", SynStep(0)); Add("step1", SynStep(1)); Add("tick", SynTick()); Add("open", SynOpen());
        Add("select", SynSelect()); Add("hit", SynHit()); Add("heart", SynHeart());
        Add("stinger", SynStinger()); Add("modem", SynModem());
        for (auto& kv : snd) TryFile(kv.first, kv.first);
        TryFile("stinger", "newgame");
        for (const char* tag : {"title", "indoors", "outdoors", "town"}) {
            for (const char* ext : {".ogg", ".mp3", ".wav"}) {
                std::string path = std::string("ee/audio/music_") + tag + ext;
                if (!FileExists(path.c_str())) continue;
                Music m = LoadMusicStream(path.c_str());
                m.looping = true; music[tag] = m; break;
            }
        }
    }
    void Play(const std::string& k, float vol = 1.0f, float pitch = 1.0f) {
        auto it = snd.find(k); if (!ready || it == snd.end()) return;
        SetSoundVolume(it->second, vol * master); SetSoundPitch(it->second, pitch); PlaySound(it->second);
    }
    void Loop(const std::string& k, float vol) {
        auto it = snd.find(k); if (!ready || it == snd.end()) return;
        if (vol <= 0.01f) { if (IsSoundPlaying(it->second)) StopSound(it->second); return; }
        SetSoundVolume(it->second, vol * master);
        if (!IsSoundPlaying(it->second)) PlaySound(it->second);
    }
    void StopAll() { for (auto& kv : snd) StopSound(kv.second); if (!cur.empty()) { StopMusicStream(music[cur]); cur.clear(); } }
    void PlayMusic(const std::string& tag) {
        if (!ready || tag == cur) return;
        if (!cur.empty()) StopMusicStream(music[cur]);
        cur.clear();
        auto it = music.find(tag);
        if (it != music.end()) { SetMusicVolume(it->second, 0.6f * master); PlayMusicStream(it->second); cur = tag; }
    }
    void Update() { if (ready && !cur.empty()) UpdateMusicStream(music[cur]); }
    void Shutdown() {
        if (!ready) return;
        StopAll();
        for (auto& kv : snd) UnloadSound(kv.second);
        for (auto& kv : music) UnloadMusicStream(kv.second);
        snd.clear(); music.clear();
        if (own) CloseAudioDevice();
        ready = own = false;
    }
    float master = 0.7f;

private:
    std::map<std::string, Sound> snd;
    std::map<std::string, Music> music;
    std::string cur;
    bool ready = false, own = false;
    void Add(const std::string& k, const Buf& b) { snd[k] = ToSound(b); }
    void TryFile(const std::string& key, const std::string& file) {
        for (const char* ext : {".wav", ".ogg", ".mp3"}) {
            std::string path = "ee/audio/" + file + ext;
            if (!FileExists(path.c_str())) continue;
            Sound s = LoadSound(path.c_str());
            if (s.frameCount == 0) continue;
            auto it = snd.find(key); if (it != snd.end()) UnloadSound(it->second);
            snd[key] = s; return;
        }
    }
};

// ====================================================================== renderer
inline const char* kWorldVS = R"c(#version 330
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;
uniform mat4 mvp;
uniform mat4 matModel;
out vec3 fragPos;
out vec2 fragTexCoord;
out vec4 fragColor;
void main() {
    // Use world coordinates so fog and flashlight falloff follow the actual room.
    fragPos = vec3(matModel * vec4(vertexPosition, 1.0));
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
)c";

// NOTE: this shader deliberately does NOT sample texture0.
// Raylib does not bind a texture when drawing DrawCube/DrawCylinder/DrawSphereEx,
// so texture0 keeps whatever was last bound (which after Present() is the render
// texture itself -> infinite fog-feedback loop). We only draw flat-coloured
// primitives anyway, so we get the colour straight from the vertex attribute.
inline const char* kWorldFS = R"c(#version 330
in vec3 fragPos;
in vec2 fragTexCoord;
in vec4 fragColor;
uniform vec4 colDiffuse;
uniform vec3 viewPos;
uniform vec3 fogColor;
uniform float fogDensity;
uniform vec3 lightPos;
uniform vec3 lightDir;
uniform float lightOn;
uniform float ambient;
out vec4 finalColor;
void main() {
    vec4 base = colDiffuse * fragColor;
    vec3 n = normalize(cross(dFdx(fragPos), dFdy(fragPos)));
    float hemi = 0.55 + 0.45 * n.y;
    vec3 toL = lightPos - fragPos;
    float d = length(toL);
    vec3 L = toL / max(d, 0.001);
    float cone = smoothstep(0.80, 0.94, dot(-L, lightDir));
    float att = 1.0 / (1.0 + 0.30 * d + 0.04 * d * d);
    float flash = lightOn * cone * att * max(abs(dot(n, L)), 0.2) * 2.6;
    vec3 lit = base.rgb * (ambient * hemi + flash);
    if (fragColor.a < 0.99) lit = base.rgb * 1.25;
    float dist = length(viewPos - fragPos);
    float f = 1.0 - exp(-pow(dist * fogDensity, 2.0));
    finalColor = vec4(mix(lit, fogColor, clamp(f, 0.0, 1.0)), 1.0);
}
)c";

inline const char* kPostFS = R"c(#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform float time;
uniform float vignette;
uniform float hurt;
uniform float desat;
out vec4 finalColor;
float hash(vec2 p) { return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453); }
void main() {
    vec2 uv = fragTexCoord;
    vec3 c = texture(texture0, uv).rgb;
    float g = dot(c, vec3(0.299, 0.587, 0.114));
    c = mix(c, vec3(g), desat);
    c *= 1.0 - smoothstep(0.32, 0.85, distance(uv, vec2(0.5))) * vignette;
    c += (hash(uv * 900.0 + vec2(time * 37.0, time * 11.0)) - 0.5) * 0.075;
    c = mix(c, c * vec3(1.5, 0.5, 0.5), hurt);
    finalColor = vec4(c, 1.0);
}
)c";

class Renderer {
public:
    static constexpr int kW = 704, kH = 448;
    ~Renderer() { Shutdown(); }
    bool Ready() const { return ok; }

    void Init() {
        world = LoadShaderFromMemory(kWorldVS, kWorldFS);
        post  = LoadShaderFromMemory(nullptr, kPostFS);
        rt    = LoadRenderTexture(kW, kH);
        TraceLog(LOG_INFO, "SH2: shaders world=%u post=%u, RT id=%u tex=%u depth=%u",
                 world.id, post.id, rt.id, rt.texture.id, rt.depth.id);
        if (rt.id == 0 || rt.texture.id == 0 || rt.depth.id == 0) {
            TraceLog(LOG_ERROR, "SH2: render texture creation failed");
            ok = false;
            return;
        }
        if (world.id == 0) TraceLog(LOG_WARNING, "SH2: world shader failed");
        if (post.id  == 0) TraceLog(LOG_WARNING, "SH2: post shader failed");
        SetTextureFilter(rt.texture, TEXTURE_FILTER_BILINEAR);
        lView = GetShaderLocation(world, "viewPos");   lFogC = GetShaderLocation(world, "fogColor");
        lFogD = GetShaderLocation(world, "fogDensity"); lLP = GetShaderLocation(world, "lightPos");
        lLD   = GetShaderLocation(world, "lightDir");   lLOn = GetShaderLocation(world, "lightOn");
        lAmb  = GetShaderLocation(world, "ambient");
        pT = GetShaderLocation(post, "time");           pV = GetShaderLocation(post, "vignette");
        pH = GetShaderLocation(post, "hurt");           pD = GetShaderLocation(post, "desat");
        ok = true;
    }
    void Shutdown() {
        if (world.id != 0) UnloadShader(world);
        if (post.id != 0) UnloadShader(post);
        if (rt.id != 0) UnloadRenderTexture(rt);
        world = {}; post = {}; rt = {}; ok = false;
    }

    void Begin(const Camera3D& cam, Color fog, float density, float ambient, Vector3 lightPos, Vector3 lightDir, float lightOn) {
        if (!ok) return;
        if (world.id != 0) {
            Vector3 fc{fog.r / 255.0f, fog.g / 255.0f, fog.b / 255.0f};
            SetShaderValue(world, lView, &cam.position, SHADER_UNIFORM_VEC3);
            SetShaderValue(world, lFogC, &fc, SHADER_UNIFORM_VEC3);
            SetShaderValue(world, lFogD, &density, SHADER_UNIFORM_FLOAT);
            SetShaderValue(world, lLP, &lightPos, SHADER_UNIFORM_VEC3);
            SetShaderValue(world, lLD, &lightDir, SHADER_UNIFORM_VEC3);
            SetShaderValue(world, lLOn, &lightOn, SHADER_UNIFORM_FLOAT);
            SetShaderValue(world, lAmb, &ambient, SHADER_UNIFORM_FLOAT);
        }
        BeginTextureMode(rt);
        ClearBackground(fog);
        BeginMode3D(cam);
        if (world.id != 0) BeginShaderMode(world);
    }

    void End() {
        if (!ok) return;
        if (world.id != 0) EndShaderMode();
        EndMode3D();
        EndTextureMode();
    }

    void Present(float time, float vignette, float hurt, float desat) {
        if (!ok) return;
        if (post.id != 0) {
            SetShaderValue(post, pT, &time,     SHADER_UNIFORM_FLOAT);
            SetShaderValue(post, pV, &vignette, SHADER_UNIFORM_FLOAT);
            SetShaderValue(post, pH, &hurt,     SHADER_UNIFORM_FLOAT);
            SetShaderValue(post, pD, &desat,    SHADER_UNIFORM_FLOAT);
            BeginShaderMode(post);
        }
        DrawTexturePro(rt.texture, {0, 0, (float)kW, -(float)kH}, {0, 0, (float)kCanvasW, (float)kCanvasH}, {0, 0}, 0, Color{255, 255, 255, 255});
        if (post.id != 0) EndShaderMode();
    }

private:
    Shader world{}, post{};
    RenderTexture2D rt{};
    int lView = 0, lFogC = 0, lFogD = 0, lLP = 0, lLD = 0, lLOn = 0, lAmb = 0, pT = 0, pV = 0, pH = 0, pD = 0;
    bool ok = false;
};

}  // namespace sh2