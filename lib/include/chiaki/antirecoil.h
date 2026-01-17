#ifndef CHIAKI_ANTIRECOIL_H
#define CHIAKI_ANTIRECOIL_H

#include <chiaki/common.h>
#include <chiaki/ctrl.h>  // ← IMPORTANTE: incluir ctrl.h
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool enabled;
    float vertical_strength;
    float horizontal_strength;
    int delay_ms;
    bool adaptive;
    uint32_t fire_button;
} AntiRecoilConfig;

CHIAKI_EXPORT void chiaki_antirecoil_init(AntiRecoilConfig *config);
CHIAKI_EXPORT void chiaki_antirecoil_process(ChiakiControllerState *state);
CHIAKI_EXPORT void chiaki_antirecoil_toggle(bool enabled);
CHIAKI_EXPORT void chiaki_antirecoil_set_vertical_strength(float strength);
CHIAKI_EXPORT void chiaki_antirecoil_set_horizontal_strength(float strength);
CHIAKI_EXPORT void chiaki_antirecoil_set_fire_button(uint32_t button);
CHIAKI_EXPORT bool chiaki_antirecoil_is_active(void);

#ifdef __cplusplus
}
#endif

#endif
