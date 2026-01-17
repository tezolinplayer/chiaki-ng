/*
 * Anti-Recoil Header for Chiaki-ng
 * Arquivo: lib/include/chiaki/antirecoil.h
 * 
 * SOLUÇÃO DEFINITIVA - Inclui ctrl.h para resolver tipos
 */

#ifndef CHIAKI_ANTIRECOIL_H
#define CHIAKI_ANTIRECOIL_H

#include <chiaki/common.h>
#include <chiaki/ctrl.h>  // CRÍTICO: Incluir para ChiakiControllerState
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Configurações do Anti-Recoil
typedef struct {
    bool enabled;
    float vertical_strength;      // 0.0 - 100.0
    float horizontal_strength;    // 0.0 - 100.0
    int delay_ms;                 // Delay antes de aplicar (0-500ms)
    bool adaptive;                // Compensação adaptativa
    uint32_t fire_button;         // R2 por padrão
} AntiRecoilConfig;

/**
 * Inicializa o sistema de anti-recoil
 * @param config Configuração personalizada ou NULL para padrão
 */
CHIAKI_EXPORT void chiaki_antirecoil_init(AntiRecoilConfig *config);

/**
 * Processa o estado do controle e aplica anti-recoil
 * @param state Estado atual do controle
 */
CHIAKI_EXPORT void chiaki_antirecoil_process(ChiakiControllerState *state);

/**
 * Ativa/desativa o anti-recoil em runtime
 * @param enabled true para ativar, false para desativar
 */
CHIAKI_EXPORT void chiaki_antirecoil_toggle(bool enabled);

/**
 * Ajusta força vertical do anti-recoil
 * @param strength Força de 0.0 a 100.0
 */
CHIAKI_EXPORT void chiaki_antirecoil_set_vertical_strength(float strength);

/**
 * Ajusta força horizontal do anti-recoil
 * @param strength Força de 0.0 a 100.0
 */
CHIAKI_EXPORT void chiaki_antirecoil_set_horizontal_strength(float strength);

/**
 * Define o botão de disparo
 * @param button Código do botão
 */
CHIAKI_EXPORT void chiaki_antirecoil_set_fire_button(uint32_t button);

/**
 * Verifica se o anti-recoil está ativo
 * @return true se ativo e disparando
 */
CHIAKI_EXPORT bool chiaki_antirecoil_is_active(void);

#ifdef __cplusplus
}
#endif

#endif // CHIAKI_ANTIRECOIL_H
