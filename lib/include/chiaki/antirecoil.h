#pragma once

// --- CORREÇÃO DE OURO: Importar ctrl.h aqui ---
#include <chiaki/ctrl.h> 
// ----------------------------------------------
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Estrutura de configuração
typedef struct {
    bool enabled;
    float vertical_strength;
    int fire_button; // Ex: CHIAKI_CONTROLLER_BUTTON_R2
} AntiRecoilConfig;

// Funções principais
void chiaki_antirecoil_init(void);
void chiaki_antirecoil_process(ChiakiControllerState *state);

#ifdef __cplusplus
}
#endif
